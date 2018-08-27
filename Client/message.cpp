#include "message.h"

void Message::setTime(QString timeStr){
    time = QTime::fromString(timeStr,"hh:mm");
    timeString = timeStr;
}
