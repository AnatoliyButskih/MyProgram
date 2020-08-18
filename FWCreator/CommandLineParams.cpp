#include "CommandLineParams.h"
#include "fwdata.h"
#include <iostream>
using namespace std;

#include <QString>

CommandLineParser& CommandLineParser::instance()
{
    static CommandLineParser cmd;
    return cmd;
}

void CommandLineParser::init(QStringList params)
{
    m_data.clear();
    for(QString &s: params) {
        if(!s.contains("=")) continue;
        QStringList list = s.split("=");
        m_data[list[0]] = list[1];
    }
}

QString CommandLineParser::getValDef(QString key, QString defVal)
{
    if(!m_data.contains(key)) return defVal;
    return m_data[key];
}

QString CommandLineParser::getVal(QString key)
{
    return m_data[key];
}

