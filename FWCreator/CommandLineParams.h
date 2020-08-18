#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <qmap.h>

//#include <Qmap>//Windows
#include <QStringList>

class CommandLineParser
{
public:

    static CommandLineParser& instance();

    void init(QStringList params);
    QString getValDef(QString key, QString defVal = "");
    QString getVal(QString key);

private:
    CommandLineParser() {}
    CommandLineParser( const CommandLineParser&);
    CommandLineParser& operator=(CommandLineParser&);


private:
    QMap<QString, QString> m_data;

};

#endif // COMMANDLINEPARSER_H
