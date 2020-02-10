#ifndef FAKE_PRODUCER_H_
#define FAKE_PRODUCER_H_

#include "producer.h"

#include <QTimer>

namespace producers {

class FakeProducer : public Producer {
    Q_OBJECT

private:
    QTimer* timer_;
    double freq_, n_, alpha_, omega_;

public:
    FakeProducer(QObject* parent);
    ~FakeProducer() override;

    bool init(json& config) override;

public slots:
    void onTimeout();
};

} // namespace

#endif