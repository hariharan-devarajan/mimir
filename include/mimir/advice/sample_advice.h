//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_SAMPLE_ADVICE_H
#define MIMIR_SAMPLE_ADVICE_H
class SampleAdvice : public Advice {
public:
    uint32 _size_mb;
    DataRepresentation _representation;

    FileAdvice() : Advice(PrimaryAdviceType::DATA_SAMPLE), _size_mb(), _representation(){}
};
#endif //MIMIR_SAMPLE_ADVICE_H
