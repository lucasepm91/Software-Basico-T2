#ifndef GERA_FUNC_H
#define GERA_FUNC_H

typedef enum {INT_PAR, CHAR_PAR, DOUBLE_PAR, PTR_PAR} TipoParam;

   typedef struct {
      TipoParam tipo;  /* indica o tipo do parâmetro */
      int amarrado;    /* indica se o parâmetro deve ter um valor amarrado */
      union {
         int v_int;
		 char v_char;
         double v_double;
         void* v_ptr;
      } valor;         /* define o valor do parâmetro se este for amarrado */
   } Parametro;

void* gera_func (void* f, int n, Parametro params[]);
void libera_func (void* func);

#endif
