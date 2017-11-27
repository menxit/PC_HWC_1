# Homework C1

## Come runnare i test
```sh
docker-compose -f docker-compose-test.yml up
```

## Come runnare l'esperimento del Back-Pressure
```sh
docker-compose -f docker-compose-backpressure.yml up
```

Il progetto fa uso di semafori. L'ambiente di sviluppo utilizzato è Mac OS. In questo OS il supporto ai
semafori pthread è limitato, per questo motivo è stata realizzata l'interfaccia sem.h (src/ginosa/sem.h).

In questa interfaccia a tempo di compilazione si controlla l'OS della macchina. Se è APPLE si fa uso dei semafori
inclusi in dispatch/dispatch.h, altrimenti si fa uso dei semafori classici inclusi in semaphore.h.

Per i test sono state create tre "classi" di comodo, in particolare:

- ConsumatoreBloccanteDiUnMessaggio
- ProduttoreBloccanteDiUnMessaggio
- Factory


## ConsumatoreBloccanteDiUnMessaggio
Questa "classe" effettua una consumazione bloccante di un messaggio all'interno di un thread. Quando ho effettuato i
test di codice concorrente ho avuto la necessità di ricevere un "segnale" che mi indicasse che la get_bloccante fosse
stata terminata. Per questo motivo ho incluso in questo Consumatore un semaforo denominato messageIsConsumed.
Questo semaforo nasce rosso, quando si supera la get_bloccante diventa verde. 


## ProduttoreBloccanteDiUnMessaggio
Questa "classe" effettua una produzione bloccante di un messaggio all'interno di un thread. Anche qui ho incluso
un semaforo denominato messageIsProduces. Questo semaforo nasce rosso, quando si supera la put_bloccante diventa verde.


## Factory
Durante i test ho trovato comodo avere a disposizione anche una "classe" Factory, questa permette di creare buffer vuoti,
pieni, mezzi pieni. Inoltre consente di creare Consumatori, Produttori e messaggi.


## Back-Pressure
In primo luogo ho avuto la necessità di creare un meccanismo che mi permettesse di far comunicare due processi remoti.
Per questo motivo ho realizzato due struct di utilità, denominate server_udp e client_udp. Queste due struct nascondono
la complessità delle socket e forniscono un'interfaccia minimale, in particolare:

### server_udp
In questo modo è possibile creare un server che rimane in ascolto di eventuali messaggi di grandezza massima pari a 512 bit
sulla porta 8888.

```c
server_udp* server = _new_server_udp(8888, 512, onMessageReceived, onMessageReceivedArgs);
server->listenConnections(server);
```

onMessageReceived è una funzione che viene richiamata nel momento in cui si riceve un messaggio, ad esempio. Questa funzione
può anche restituire un valore. Il valore restituito viene inviato al client dal quale si è ricevuto il messaggio:
```c
char* onMessageReceived(char* fromAddress, unsigned short int port, char* message, void* args) {
    printf("Messaggio inviato da %s:%d: %message", frromAddress, port, message);
    return "ok";
}
```

Nel dettaglio, server_udp non è altro che un loop infinito. All'interno di questo loop infinito c'è una chiamata
bloccante che attende la ricezione di un messaggio. Quando la chiamata bloccante viene superata (e quindi quando è stato
ricevuto un messaggio), viene invocata la funzione onMessageReceived, passandogli come parametro attuale le informazioni
della connessione (indirizzo ip e porta del processo remoto) e il contenuto del messaggio stesso.

### client_udp
La controparte di server_udp è client_udp. Questa struct permette di instaurare una connessione con un server_udp e di
inviargli dei messaggi.

```c
client_udp* client = _new_client_udp("127.0.0.1", 8888, 512);
client->openConnection(client);
```

A questo punto per inviare un messaggio basta richiamare sendMessage. La funzione sendMessage è una chiamata bloccante e
restituisce un valore. Il valore restituito corrisponde alla risposta del server una volta ricevuto il pacchetto.
```c
char* response = client->sendMessage(client, "sono un messaggio");
```

### Nodi e Code
A questo punto si hanno a disposizione gli strumenti necessari a far comunicare processi remoti. Ora un possibile
scenario problematico è il seguente.

```bash
n1_veloce -> coda_1 -> n2_lento -> coda_2 -> n3_veloce
```

n1_veloce, n2_lento e n3_veloce sono nodi. n1_veloce produce messaggi con throughput elevato, n2_lento con throughput
lento e n3_veloce consuma messaggi con throughput elevato.

coda_1 e coda_2 invece sono buffer di dimensione limitata (E.G. N).

A questo punto se il primo nodo produce e invia messaggi con un throughput elevato e il secondo nodo invece consuma ed
estrae messaggi con throughput lento, ad un certo punto si supera la dimensione massima consentita da coda_1.

Serve quindi un meccanismo di regolazione, ovvero qualche cosa che comunichi a n1_veloce di produrre messaggi più lentamente.

A questo punto l'idea è questa. Un nodo invia un messaggio alla coda. A questo punto può ricevere due risposte diverse: 
MESSAGGIO_INSERITO ed ERRORE_MESSAGGIO_NON_INSERITO.

Se si riceve come risposta MESSAGGIO_INSERITO il produttore si occuperà di produrre il successivo messaggio e di inviarlo
nuovamente alla coda. In caso contrario, il produttore prova a inviare nuovamente il messaggio, fino a quando non riceve
la risposta MESSAGGIO_INSERITO. In questo modo il produttore si autoregola.

Dall'altra parte, il secondo nodo (quello che consuma) invia periodicamente alla coda una richiesta di consumazione. Se
la coda contiene un messaggio, restituisce il messaggio al nodo e lo rimuove dal buffer, se invece non contiene alcun
tipo di messaggio risponde con MESSAGGIO_NON_PRESENTE.

Dal punto di vista della coda, quando essa riceve una richiesta di inserimento da un nodo, essa tenta semplicemente di 
effettuare una put_non_bloccante sul buffer. Se riesce a inserire il messaggio risponde con MESSAGGIO_INSERITO, altrimenti
risponde con ERRORE_MESSAGGIO_NON_INSERITO.

Quando invece riceve una richiesta di estrazione messaggio, quel che si fa è semplicemente una get_non_bloccante.

Per implementare code e nodi sono state create due struct di comodo:
- mx_node
- mx_queue

Nel seguente modo si crea una coda, in ascolto di messaggi sulla porta 8888. Questa coda può contenere al massimo 100 
messaggi ed è denominata "CODA_1".
```c
mx_queue* queue = _new_mx_queue(8888, 100, "CODA_1");
queue->start(queue);  
```

Nel seguente modo invece si può creare un nodo che produce infiniti messaggi e li invia sulla coda sull'indirizzo
127.0.0.1 porta 8888.

La funzione publish è bloccante. Questa funzione prende il content del messaggio, lo wrappa in un pacchetto di tipo
INSERT e prova a inviare il pacchetto alla coda fino a che non riceve una risposta positiva.
```c
mx_node* node = _new_mx_node(10);
node->createPublisher(node, "127.0.0.1", 8888);
while(1) {
    msg_t* m1 = produci_messaggio();
    node->publish(node, m1);
}
```

In questo modo invece si crea un nodo che consuma messaggi da una coda. Quando riceve un messaggio lo rielabora e 
successivamente lo pubblica sulla successiva coda:
```c
void onMessageReceived(mx_node* this, msg_t* msg) {
  printf ("RICEVUTO: %s\n", (char*)msg->content);
  // elaborazione del messaggio
  this->publish(this, msg);
}

msg_t* m1 = msg_init("another message");
mx_node* node = _new_mx_node(10);
node->createPublisher(node, getAddressPublisher(), getPortPublisher());
node->createSubscriber(node, getAddressSubscriber(), getPortSubscriber(), onMessageReceived);
```
