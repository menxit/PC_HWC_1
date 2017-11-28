#include <CUnit/Basic.h>
#include "all_tests/all_tests.h"

int main(int argc, char **argv) {

  CU_pSuite pSuite = NULL;

  // initialize the CUnit test registry
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  // add a suite to the registry
  pSuite = CU_add_suite("HWC1 Suite 1", NULL, NULL);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  // add the tests to the suite
  if (
    NULL == CU_add_test(pSuite,
                        "✓ Produzione non bloccante in un buffer già pieno",
                        test_produzioneNonBloccateInUnBufferGiaPieno) ||
      NULL == CU_add_test(pSuite,
                          "✓ Consumazione bloccante da un buffer inizialmente vuoto",
                          test_consumazioneBloccanteDaUnBufferInizialmenteVuoto) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P=1; C=0; N=1) Produzione di un solo messaggio in un buffer vuoto",
                          test_produzioneDiUnSoloMessaggioInUnBufferVuoto) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P=0; C=1; N=1) Consumazione di un solo messaggio da un buffer pieno",
                          test_consumazioneDiUnSoloMessaggioDaUnBufferPieno) ||
      NULL == CU_add_test(pSuite, "✓ (P=0; C=1; N=1) Consumazione da un buffer vuoto", test_consumazioneDaUnBufferVuoto)
      ||
        NULL == CU_add_test(pSuite, "✓ (P=1; C=0; N=1) Produzione in un buffer pieno", test_produzioneInUnBufferPieno)
      ||
        NULL == CU_add_test(pSuite,
                            "✓ (P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio da un buffer unitario; prima il consumatore",
                            test_consumazioneEProduzioneConcorrenteDiUnMessaggioDaUnBufferUnitarioPrimaIlConsumatore) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio in un buffer unitario; prima il produttore",
                          test_consumazioneEProduzioneConcorrenteDiUnMessaggioInUnBufferUnitarioPrimaIlProduttore) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P>1; C=0; N=1) Produzione concorrente di molteplici messaggi in un buffer unitario vuoto",
                          test_produzioneConcorrenteDiMoltepliciMessaggiInUnBufferUnitarioVuoto) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P=0; C>1; N=1) Consumazione concorrente di molteplici messaggi da un buffer unitario pieno",
                          test_consumazioneConcorrenteDiMoltepliciMessaggiDaUnBufferUnitarioPieno) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un buffer vuoto; il buffer non si riempe",
                          test_produzioneConcorrenteDiMoltepliciMessaggiInUnBufferVuotoIlBufferNonSiRiempie) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un buffer pieno; il buffer è già saturo",
                          test_produzioneConcorrenteDiMoltepliciMessaggiInUnBufferPienoIlBufferEGiaSaturo) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un buffer vuoto; il buffer si satura in corso",
                          test_produzioneConcorrenteDiMoltepliciMessaggiInUnBufferVuotoIlBufferSiSaturaInCorso) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P=0; C>1; N>1) Consumazione concorrente di molteplici messaggi da un buffer pieno",
                          test_consumazioneConcorrenteDiMoltepliciMessaggiDaUnBufferPieno) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P>1; C>1; N=1) Consumazioni e produzioni concorrenti di molteplici messaggi in un buffer unitario",
                          test_consumazioniEProduzioniConcorrenteDiMoltepliciMessaggiInUnBufferUnitario) ||
      NULL == CU_add_test(pSuite,
                          "✓ (P>1; C>1; N>1) Consumazioni e produzioni concorrenti di molteplici messaggi in un buffer",
                          test_consumazioniEProduzioniConcorrenteDiMoltepliciMessaggiInUnBuffer) ||
      NULL == CU_add_test(pSuite, "✓ Copiare un messaggio intero", test_msgCopyConUnIntero) ||
      NULL == CU_add_test(pSuite, "✓ Copiare un messaggio string", test_msgCopyConUnaString)) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Run all tests using the CUnit Basic interface
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();

  return CU_get_error();
}

