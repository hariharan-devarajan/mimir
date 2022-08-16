//
// Created by haridev on 2/16/22.
//

#ifndef MIMIR_API_APPLICATION_H
#define MIMIR_API_APPLICATION_H

#include <mimir/advice/application_advice.h>
#include <mimir/typedef.h>

namespace mimir {
MimirStatus application_advice_begin(ApplicationAdvice &payload,
                                     MimirHandler &handler);
MimirStatus application_advice_end(MimirHandler &handler);
MimirStatus free_applications();
}  // namespace mimir

#endif  // MIMIR_API_APPLICATION_H
