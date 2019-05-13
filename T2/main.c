   #include <math.h>
   #include <stdio.h>
   #include "gera_func.h"

   typedef int (*func_ptr) (int);
   
int teste (int a, int *b)
{	
	int aux;
	aux = a + *b;
	return aux;
}

   int main (void) {
      Parametro params[2];
      func_ptr f_quadrado = NULL;      
      int i,c = 2;

      params[0].tipo = INT_PAR;
      params[0].amarrado = 0;
      params[1].tipo = PTR_PAR;
      params[1].amarrado = 1;
      params[1].valor.v_ptr = &c;

      f_quadrado = (func_ptr) gera_func (teste, 2, params);

      
      i = f_quadrado(7);
      printf("i:%d\n",i);
      

      libera_func(f_quadrado);
      return 0;
   }
