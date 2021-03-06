#ifndef _IO_ADAPTER_H_
#define _IO_ADAPTER_H_

#include <cstdio>

class FileOutputAdapter
{
private:
    std::FILE *m_fout;
public:
    FileOutputAdapter(FILE *f)
        :m_fout(f) {
    }
    
    void operator() (int ch) {
        std::fputc(ch, m_fout);
    }
};

class NullOutputAdapter
{
private:
    int m_count;
public:
    NullOutputAdapter()
        :m_count(0) {
    }

    int count() {
        return m_count;
    }
    
    void operator() (int ch) {
        ++m_count;
    }
};

class FileInputAdapter
{
private:
    std::FILE *m_fin;
public:
    FileInputAdapter(FILE *f)
        :m_fin(f) {
    }
    
    int operator() () {
        return std::fgetc(m_fin);
    }
};

#endif /* _IO_ADAPTER_H_ */
