#ifndef HC_C1_FACTORY_H
#define HC_C1_FACTORY_H

#define EXPECTED_MSG 1010

#include "../ConsumatoreBloccanteDiUnMessaggio/ConsumatoreBloccanteDiUnMessaggio.h"
#include "../ProduttoreBloccanteDiUnMessaggio/ProduttoreBloccanteDiUnMessaggio.h"

msg_t *factory_createExpectedMessage();
buffer_t *factory_createEmptyBuffer(unsigned int sizeBuffer);
buffer_t *factory_createFullBuffer(unsigned int sizeBuffer);
buffer_t *factory_createHalfFullBuffer(unsigned sizeBuffer);
ConsumatoreBloccanteDiUnMessaggio *factory_createConsumatoreBloccanteDiUnMessaggio(buffer_t *buffer);
ProduttoreBloccanteDiUnMessaggio *factory_createProduttoreBloccanteDiUnMessaggio(buffer_t *buffer);

#endif
