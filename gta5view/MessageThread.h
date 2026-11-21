#ifndef MESSAGETHREAD_H
#define MESSAGETHREAD_H
#include <QJsonObject>
#include <QObject>
#include <QThread>
class MessageThread : public QThread
{
    Q_OBJECT
public:
    explicit MessageThread(uint cacheId, QObject *parent = 0);
public slots:
    void terminateThread();
private:
    bool threadRunning;
    uint cacheId;
protected:
    void run();
signals:
    void messagesArrived(const QJsonObject &messageObject);
    void updateCacheId(uint cacheId);
    void threadTerminated();
};
#endif