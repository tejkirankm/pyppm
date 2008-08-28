#ifndef _PPM_MODEL_H_
#define _PPM_MODEL_H_

#include <cstring>

#include "config.h"
#include "buffer.h"
#include "trie.h"
#include "arithmetic_encoder.h"
#include "arithmetic_decoder.h"

class PPMModel
{
protected:
    typedef Trie<symbol_t> Trie_t;
    Trie_t m_contexts[Max_no_contexts+1];
    Buffer<symbol_t> m_buffer;

    void update_contexts(int ictx, symbol_t sym) {
        int offset = m_buffer.length() - ictx - 1;
        while (offset >= 0) {
            m_contexts[ictx].update_model(m_buffer, offset, sym);
            offset--;
            ictx++;
        }
    }
};

class PPMEncoder: public PPMModel
{
private:
    ArithmeticEncoder *m_encoder;

    void uni_encode(symbol_t sym) {
        m_encoder->encode(sym, sym+1, No_of_chars);
        update_contexts(1, sym);
    }

public:
    PPMEncoder(ArithmeticEncoder *encoder)
        :m_encoder(encoder) { }

    void start_encoding() {
        // Do nothing
    }
    
    void encode(symbol_t sym) {
        int ictx = m_buffer.length();

        if (ictx == 0) {
            uni_encode(sym);
        } else {
            
            for (int i = 0; ictx > 0; --ictx, ++i) {
                if (m_contexts[ictx].encode(m_encoder, m_buffer, i, sym)) {
                    update_contexts(ictx+1, sym);
                    break;  // predict success
                }
            }

            uni_encode(sym);
        }
    }

    void finish_encoding() {
        m_encoder->finish_encoding();
    }
};

class PPMDecoder: public PPMModel
{
private:
    ArithmeticDecoder *m_decoder;

    wsymbol_t uni_decode() {
        code_value cum;
        symbol_t sym;
        cum = m_decoder->get_cum_freq(No_of_chars);
        assert(cum > 0 && cum <= No_of_chars);
        sym = cum-1;

        update_contexts(1, sym);
        return sym;
    }
    
public:
    PPMDecoder(ArithmeticDecoder *decoder)
        :m_decoder(decoder) { }

    void start_decoding() {
        m_decoder->start_decoding();
    }
    
    wsymbol_t decode() {
        int ictx = m_buffer.length();
        wsymbol_t symbol;

        if (ictx == 0) {
            return uni_decode();
        } else {
            for (int i = 0; ictx >= 0; --ictx, ++i) {
                symbol = m_contexts[ictx].decode(m_decoder, m_buffer, i);
                if (symbol != ESC_symbol) {
                    update_contexts(ictx+1, symbol);
                    return symbol;
                }
            }

            return uni_decode();
        }
    }

    void finish_decoding() {
        // Do nothing
    }
};


#endif /* _PPM_MODEL_H_ */
