#include <CUnit/CUnit.h>
#include <buffer_t/buffer_t.h>
#include <stdlib.h>
#include "../utilities/Factory/Factory.h"

/**
 * Questa funzione applica un ritardo pari a DELAY + un numero random tra 0 e DELAY/10.
 * Si applica un ritardo randomico per evitare eventuali strane serializzazioni da parte
 * dell'OS.
 */
void randomDelay() {
  int DELAY = 1000;
  usleep(DELAY + (rand() % (DELAY / 10)));
}

void test_produzioneNonBloccateInUnBufferGiaPieno() {
  // creo un buffer di dimensione 1 pieno
  buffer_t *buffer_pieno = factory_createFullBuffer(1);

  // creo messaggio da inserire
  msg_t *msg = factory_createExpectedMessage();

  // inserisco il messaggio nel buffer con la put non bloccante
  msg_t *result = put_non_bloccante(buffer_pieno, msg);

  // dovrebbe restituire BUFFER_ERROR perché il buffer è pieno
  CU_ASSERT_EQUAL(result, BUFFER_ERROR);
}

void test_consumazioneBloccanteDaUnBufferInizialmenteVuoto() {
  // creo un buffer di dimensione 1 vuoto
  buffer_t *buffer_vuoto = factory_createEmptyBuffer(1);

  // creo messaggio da inserire
  msg_t *msg = factory_createExpectedMessage();

  // inserisco nel buffer vuoto un messaggio
  msg_t *result = put_non_bloccante(buffer_vuoto, msg);

  // non dovrebbe restituire errore perché il buffer è vuoto
  CU_ASSERT_NOT_EQUAL(result, BUFFER_ERROR);

  // dovrebbe restituirmi un EXPECTED_MSG
  CU_ASSERT_EQUAL(*(int *) (result->content), EXPECTED_MSG);
}

/**
 *  (P=1; C=0; N=1) Produzione di un solo messaggio in un buffer vuoto
 */
void test_produzioneDiUnSoloMessaggioInUnBufferVuoto() {
  // creo un buffero vuoto di dimensione 1
  buffer_t *buffer_vuoto = factory_createEmptyBuffer(1);

  // creo un produttore bloccante di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p = factory_createProduttoreBloccanteDiUnMessaggio(buffer_vuoto);

  // delay
  randomDelay();

  // a questo punto il messaggio dovrebbe essere stato prodotto
  CU_ASSERT_EQUAL(pc_sem_trywait(&p->messageIsProduced), 0);
}

/**
 *  (P=0; C=1; N=1) Consumazione di un solo messaggio da un buffer pieno
 */
void test_consumazioneDiUnSoloMessaggioDaUnBufferPieno() {
  // creo un buffer pieno di dimensione 1
  buffer_t *buffer_pieno = factory_createFullBuffer(1);

  // creo un consumatore bloccante di un messaggio
  ConsumatoreBloccanteDiUnMessaggio *c = factory_createConsumatoreBloccanteDiUnMessaggio(buffer_pieno);

  // applico un delay per essere certo che il consumatore abbia fatto in tempo a consumare il messaggio
  randomDelay();

  // a questo punto il messaggio dovrebbe essere stato consumato
  CU_ASSERT_EQUAL(pc_sem_trywait(&c->messageIsConsumed), 0);
}

/**
 * (P=1; C=0; N=1) Produzione in un buffer pieno
 */
void test_produzioneInUnBufferPieno() {
  // creo un buffer pieno di dimensione 1 vuoto
  buffer_t *buffer = factory_createFullBuffer(1);

  // creo un produttore bloccante di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // applico un delay, anche se in realtà il produttore dovrebbe
  // rimanere bloccato comunque.
  randomDelay();

  // il messaggio dovrebbe ancora non essere stato prodotto, perché
  // il buffer è pieno
  CU_ASSERT_FALSE(pc_sem_trywait(&p->messageIsProduced) == 0);

  // estraggo un messaggio
  get_bloccante(buffer);

  // applico un delay per essere certo che
  // pc_sem_trywait non venga fatta tra la put_bloccante pc_sem_post di messageIsProduced
  randomDelay();

  // a questo punto il messaggio dovrebbe essere stato prodotto
  CU_ASSERT_EQUAL(pc_sem_trywait(&p->messageIsProduced), 0);
}

/**
 * (P=0; C=1; N=1) Consumazione da un buffer vuoto
 */
void test_consumazioneDaUnBufferVuoto() {
  // creo un buffer di dimensione 1 vuoto
  buffer_t *buffer = factory_createEmptyBuffer(1);

  // creo un consumatore bloccante di un messaggio
  ConsumatoreBloccanteDiUnMessaggio *c = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);

  // applico un delay, per essere certo che il consumatore sia realmente bloccato
  randomDelay();

  // se il messaggio non è stato ancora consumato, probabilmente il consumatore è bloccato
  CU_ASSERT_FALSE(pc_sem_trywait(&c->messageIsConsumed) == 0);

  // creo un produttore bloccante
  ProduttoreBloccanteDiUnMessaggio *p = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // applico un delay per essere certo che il produttore abbia fatto in tempo
  // a produrre il messaggio e metterlo sul buffer
  randomDelay();

  // a questo punto il messaggio dovrebbe essere stato consumato
  CU_ASSERT_EQUAL(pc_sem_trywait(&c->messageIsConsumed), 0);
}

/**
 *  (P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio
 *  da un buffer unitario; prima il consumatore
 */
void test_consumazioneEProduzioneConcorrenteDiUnMessaggioDaUnBufferUnitarioPrimaIlConsumatore() {
  // creo un buffer di dimensione 1 vuoto
  buffer_t *buffer = factory_createEmptyBuffer(1);

  // creo un consumatore bloccante di un messaggio
  ConsumatoreBloccanteDiUnMessaggio *c = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);

  // creo un produttore bloccante di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // faccio una sleep per essere certo che il produttore abbiano fatto in tempo a produrre il messaggio
  // e che il consumatore abbia fatto in tempo a consumarlo
  randomDelay();

  CU_ASSERT_EQUAL(pc_sem_trywait(&c->messageIsConsumed), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&p->messageIsProduced), 0);
}

/**
 *  (P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio
 *  in un buffer unitario; prima il produttore
 *
 *  N.B. Questo test in realtà risulta poco utile, in quanto eseguire prima il produttore
 *  o prima il consumatore non cambia nulla.
 */
void test_consumazioneEProduzioneConcorrenteDiUnMessaggioInUnBufferUnitarioPrimaIlProduttore() {

  // creo un buffer di dimensione 1 vuoto
  buffer_t *buffer = factory_createEmptyBuffer(1);

  // creo un produttore bloccante di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // creo un consumatore bloccante di un messaggio
  ConsumatoreBloccanteDiUnMessaggio *c = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);

  // faccio una sleep per essere certo che il produttore abbiano fatto in tempo a produrre il messaggio
  // e che il consumatore abbia fatto in tempo a consumarlo
  randomDelay();

  CU_ASSERT_EQUAL(pc_sem_trywait(&c->messageIsConsumed), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&p->messageIsProduced), 0);
}

/**
 * (P>1; C=0; N=1) Produzione concorrente di molteplici messaggi in un buffer unitario vuoto
 */
void test_produzioneConcorrenteDiMoltepliciMessaggiInUnBufferUnitarioVuoto() {
  // creo un buffer di dimensione 1 vuoto
  buffer_t *buffer = factory_createEmptyBuffer(1);

  // creo molteplici produttori bloccanti di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p1 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p2 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p3 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // faccio una sleep per essere certo che almeno un produttore
  // abbia fatto in tempo a produrre il messaggio
  randomDelay();

  long result = pc_sem_trywait(&p1->messageIsProduced) == 0;
  result += pc_sem_trywait(&p2->messageIsProduced) == 0;
  result += pc_sem_trywait(&p3->messageIsProduced) == 0;

  // Controllo che result sia uguale a 1, ovvero che almeno un produttore (non so quale)
  // sia riuscito a mettere il messaggio sul buffer.
  CU_ASSERT_EQUAL(result, 1);
}

/**
 * (P=0; C>1; N=1) Consumazione concorrente di molteplici messaggi da un buffer unitario pieno
 */
void test_consumazioneConcorrenteDiMoltepliciMessaggiDaUnBufferUnitarioPieno() {

  // creo un buffer di dimensione 1 pieno
  buffer_t *buffer = factory_createFullBuffer(1);

  // creo molteplici consumatori bloccanti di un messaggio
  ConsumatoreBloccanteDiUnMessaggio *c1 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);
  ConsumatoreBloccanteDiUnMessaggio *c2 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);
  ConsumatoreBloccanteDiUnMessaggio *c3 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);

  // faccio una sleep per essere certo che almeno un consumatore
  // abbia fatto in tempo a produrre il messaggio
  randomDelay();

  long result = pc_sem_trywait(&c1->messageIsConsumed) == 0;
  result += pc_sem_trywait(&c2->messageIsConsumed) == 0;
  result += pc_sem_trywait(&c3->messageIsConsumed) == 0;

  // Controllo che result sia uguale a 1, ovvero che almeno un consumatore (non so quale)
  // sia riuscito a mettere il messaggio sul buffer.
  CU_ASSERT_EQUAL(result, 1);
}

/**
 * (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un buffer vuoto;
 * il buffer non si riempe
 */
void test_produzioneConcorrenteDiMoltepliciMessaggiInUnBufferVuotoIlBufferNonSiRiempie() {

  // creo un buffer di dimensione 10 vuoto
  buffer_t *buffer = factory_createEmptyBuffer(10);

  // creo molteplici produttori bloccanti di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p1 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p2 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p3 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // faccio una sleep per essere certo che tutti i produttori
  // abbiano fatto in tempo a produrre il messaggio
  randomDelay();

  CU_ASSERT_EQUAL(pc_sem_trywait(&p1->messageIsProduced), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&p2->messageIsProduced), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&p3->messageIsProduced), 0);
}

/**
 * (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un buffer pieno;
 * il buffer è già saturo
 */
void test_produzioneConcorrenteDiMoltepliciMessaggiInUnBufferPienoIlBufferEGiaSaturo() {

  // creo un buffer di dimensione 10 pieno
  buffer_t *buffer = factory_createFullBuffer(10);

  // creo molteplici produttori bloccanti di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p1 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p2 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p3 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // faccio una sleep per essere certo che i produttori siano bloccati
  randomDelay();

  CU_ASSERT_FALSE(pc_sem_trywait(&p1->messageIsProduced) == 0);
  CU_ASSERT_FALSE(pc_sem_trywait(&p2->messageIsProduced) == 0);
  CU_ASSERT_FALSE(pc_sem_trywait(&p3->messageIsProduced) == 0);
}

/**
 * (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un buffer vuoto;
 * il buffer si satura in corso
 */
void test_produzioneConcorrenteDiMoltepliciMessaggiInUnBufferVuotoIlBufferSiSaturaInCorso() {

  // creo un buffer di dimensione 2 vuoto
  buffer_t *buffer = factory_createEmptyBuffer(2);

  // creo molteplici produttori bloccanti di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p1 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p2 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p3 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // faccio la sleep per dare il tempo a tutti i produttori che possono produrre di produrre
  randomDelay();

  long result = pc_sem_trywait(&p1->messageIsProduced) == 0;
  result += pc_sem_trywait(&p2->messageIsProduced) == 0;
  result += pc_sem_trywait(&p3->messageIsProduced) == 0;

  CU_ASSERT_EQUAL(result, 2);
}

/**
 * (P=0; C>1; N>1) Consumazione concorrente di molteplici messaggi da un buffer pieno
 */
void test_consumazioneConcorrenteDiMoltepliciMessaggiDaUnBufferPieno() {

  // creo un buffer pieno di dimensione 2
  buffer_t *buffer = factory_createFullBuffer(2);

  // creo molteplici produttori bloccanti di un messaggio
  ConsumatoreBloccanteDiUnMessaggio *c1 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);
  ConsumatoreBloccanteDiUnMessaggio *c2 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);
  ConsumatoreBloccanteDiUnMessaggio *c3 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);

  // faccio la sleep per dare il tempo a tutti i consumatori che possono consumare di consumare.
  randomDelay();

  long result = pc_sem_trywait(&c1->messageIsConsumed) == 0;
  result += pc_sem_trywait(&c2->messageIsConsumed) == 0;
  result += pc_sem_trywait(&c3->messageIsConsumed) == 0;

  // mi aspetto due perché il buffer ha dimensione 2 e quindi un consumatore deve rimanere bloccato
  CU_ASSERT_EQUAL(result, 2);
}

/**
 * (P>1; C>1; N=1) Consumazioni e produzioni concorrenti di molteplici messaggi in un buffer unitario
 */
void test_consumazioniEProduzioniConcorrenteDiMoltepliciMessaggiInUnBufferUnitario() {

  // creo un buffer vuoto di dimensione 1
  buffer_t *buffer = factory_createEmptyBuffer(1);

  // creo molteplici produttori bloccanti di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p1 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p2 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p3 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // creo molteplici consumatori bloccanti di un messaggio
  ConsumatoreBloccanteDiUnMessaggio *c1 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);
  ConsumatoreBloccanteDiUnMessaggio *c2 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);
  ConsumatoreBloccanteDiUnMessaggio *c3 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);

  // faccio la sleep per dare a tutti i produttori/consumatori il tempo necessario
  // per inserire o estrarre messaggi
  randomDelay();

  CU_ASSERT_EQUAL(pc_sem_trywait(&p1->messageIsProduced), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&p2->messageIsProduced), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&p3->messageIsProduced), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&c1->messageIsConsumed), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&c2->messageIsConsumed), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&c3->messageIsConsumed), 0);
}

/**
 * (P>1; C>1; N>1) Consumazioni e produzioni concorrenti di molteplici messaggi in un buffer
 */
void test_consumazioniEProduzioniConcorrenteDiMoltepliciMessaggiInUnBuffer() {

  // creo un buffer mezzo pieno di dimensione 100
  buffer_t *buffer = factory_createHalfFullBuffer(100);

  // creo molteplici produttori bloccanti di un messaggio
  ProduttoreBloccanteDiUnMessaggio *p1 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p2 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);
  ProduttoreBloccanteDiUnMessaggio *p3 = factory_createProduttoreBloccanteDiUnMessaggio(buffer);

  // creo molteplici consumatori bloccanti di un messaggio
  ConsumatoreBloccanteDiUnMessaggio *c1 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);
  ConsumatoreBloccanteDiUnMessaggio *c2 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);
  ConsumatoreBloccanteDiUnMessaggio *c3 = factory_createConsumatoreBloccanteDiUnMessaggio(buffer);

  // faccio la sleep per dare a tutti i produttori/consumatori il tempo necessario
  // per fare quel che possono fare.
  randomDelay();

  CU_ASSERT_EQUAL(pc_sem_trywait(&p1->messageIsProduced), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&p2->messageIsProduced), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&p3->messageIsProduced), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&c1->messageIsConsumed), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&c2->messageIsConsumed), 0);
  CU_ASSERT_EQUAL(pc_sem_trywait(&c3->messageIsConsumed), 0);
}

void test_msgCopyConUnIntero() {
  msg_t *msg1 = factory_createExpectedMessage();
  msg_t *msg2 = msg1->msg_copy(msg1);
  CU_ASSERT_EQUAL(*(int *) msg1->content, *(int *) msg2->content);
  CU_ASSERT_EQUAL(msg1->content, msg2->content);
  CU_ASSERT_NOT_EQUAL(msg1, msg2);
}

void test_msgCopyConUnaString() {
  char *message = "MESSAGE BUFFER";
  msg_t *msg1 = msg_init(message);
  msg_t *msg2 = msg1->msg_copy(msg1);
  CU_ASSERT(strcmp(msg1->content, msg2->content) == 0);
}

