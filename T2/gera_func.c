/*
Lucas Eduardo Pereira Martins
Carlos Alves Loriano Junior 
*/

#include <stdio.h>
#include <stdlib.h>
#include "gera_func.h"


void* gera_func (void* f, int n, Parametro params[])
{				   /* a variável total será usada para calcular o espaço */
	int i,j,k,tot = 12; /* 12, pois inicio = 3, fim = 4 e call = 5 */
	unsigned int aux,res,temp;
	unsigned char *a;
	unsigned char inicio[] = {0x55,0x89,0xe5}; /* push %ebp = 55;mov %esp,%ebp = 89 e5 */
	unsigned char fim[] = {0x89,0xec,0x5d,0xc3}; /* mov %ebp,%esp = 89 ec;pop %ebp = 5d;ret = c3 */
	int p,auxp,ptr_a;		/* variável para descobrir representação de valor amarrado */
	unsigned char pilha_rec = 4; 		/* descobrir valor antes de (%ebp),para acessar */	
	unsigned char p_c,*aux_double;	

	for(i=0;i<n;i++)  /* percorrer params[] para calcular o espaço de a */
	{
	   if(params[i].tipo == INT_PAR)
	   {
			if(params[i].amarrado == 0)
			{		   
				tot += 3;     /* mov 8(%ebp),%ecx : 8b 4d 08; usa 3 bytes */
				tot += 1;	 /* push %ecx */
				pilha_rec += 4;
			}
			else		/* params[i].amarrado == 1 */
			{		   
				tot += 5;	 /* mov $0,%ecx : b9 00 00 00 00; usa 5 bytes */
				tot += 1;	 /* push %ecx */		  
			}		
	   }
	   else
	     if(params[i].tipo == DOUBLE_PAR)
	     {
			if(params[i].amarrado == 0)
			{
		       tot += 3;  /* mov 8(%ebp),%ecx : 8b 4d 04 */
		       tot += 1;	 /* push %ecx */
		       tot += 3;  /* mov 8(%ebp),%ecx : 8b 4d 04 */
		       tot += 1;	 /* push %ecx */		       
		       pilha_rec += 8;
			}
			else
			{
				tot += 12;			
			}		 
	     }
		 else
			 if(params[i].tipo == CHAR_PAR)
			 {
				 if(params[i].amarrado == 1)
					 tot += 6;
				 else
				 {
					 tot += 4;
					 pilha_rec += 4;
				 }
			 }
			 else	/* params[i].tipo == PTR_PAR */
			 {
				if(params[i].amarrado == 0)
				{		       
					tot += 3;     /* mov 8(%ebp),%ecx : 8b 4d 08; usa 3 bytes */
					tot += 1;    /* push %ecx */
					pilha_rec += 4;
				}
				else	/* params[i].amarrado == 1 */
				{		       
					tot += 5;	 /* mov $0,%ecx : b9 00 00 00 00; usa 5 bytes */
					tot += 1;	 /* push %ecx */		       
				}		   
			}
	 } /* Ao final dessa repetição tot tem o valor do tamanho de a[] */
		
	a = (unsigned char*)malloc(tot*sizeof(unsigned char));

	for(j=0;j < 3;j++)        /* começo de toda função em assembly */
	{
	   a[j] = inicio[j];
	}

	for(i=n-1;i > -1;i--)  /* colocar o código de máquina em a */
	{
	   if(params[i].tipo == INT_PAR)
	   {
	      if(params[i].amarrado == 0)
	      {		 
	         a[j] = 0x8b;
	         a[j+1] = 0x4d;
			 a[j+2] = pilha_rec;	/* descobrir esse valor: mov ?(%ebp),%ecx : 8b 4d ? */
			 pilha_rec -= 4;		
	         a[j+3] = 0x51;	/* push %ecx */
			 j += 4;
	      }
	      else	/* params[i].amarrado == 1 */
	      {
				p = params[i].valor.v_int;
				a[j] = 0xb9;		/* mov $0,%ecx : b9 00 00 00 00 */
				auxp = p << 24;	/* o valor amarrado em little-endian */
				auxp >>= 24;
				a[j+1] = auxp;
				auxp = p << 16;
				auxp >>= 24;
				a[j+2] = auxp;
				auxp = p << 8;
				auxp >>= 24;
				a[j+3] = auxp;
				auxp = p >> 24;
				a[j+4] = auxp;
				a[j+5] = 0x51;		/* push %ecx */	 
				j += 6;
	      }
	   }
	   else
	     	if(params[i].tipo == DOUBLE_PAR)
			{
				if(params[i].amarrado == 0)
				{		     
					a[j] = 0x8b;	/* mov 8(%ebp),%ecx : 8b 4d 08 */
					a[j+1] = 0x4d;
					a[j+2] = pilha_rec;
					pilha_rec -= 4;
					a[j+3] = 0x51;	/* push %ecx */	
					a[j+4] = 0x8b;	/* pegar os outros bytes */
					a[j+5] = 0x4d;
					a[j+6] = pilha_rec;
					pilha_rec -=4;
					a[j+7] = 0x51;	/* push %ecx */	
					j += 8;
				}
				else
				{
					a[j] = 0xb9;
					j++;
					aux_double = (unsigned char*)&(params[i].valor.v_double);
					for (k=4; k<8; k++, j++) /* mov $0, %ecx */
					{
						a[j] = aux_double[k];						
					}
					a[j] = 0x51; /* push %ecx */
					j++;
					a[j] = 0xb9; /* mov $0, %ecx */
					j++;

					for (k=0; k<4; k++, j++) /* mov $0, %ecx; os outros bytes */
					{
						a[j] = aux_double[k];						
					}
					a[j] = 0x51; /* push %ecx */
					j++;
				}
			}
				else
					if(params[i].tipo == CHAR_PAR)
					{
						if(params[i].amarrado == 0)
						{
							a[j] = 0x8b;
							a[j+1] = 0x4d;
							a[j+2] = pilha_rec;/* mov ?(%ebp),%ecx : 8b 4d ? */
							pilha_rec -= 4;		
							a[j+3] = 0x51;	/* push %ecx */
							j += 4;
						}
						else
						{
							p_c = params[i].valor.v_char;
							a[j] = 0xb9;		/* mov $0,%ecx : b9 00 00 00 00 */
							a[j+1] = p_c;	/* o valor amarrado em little-endian */
							a[j+2] = 0;
							a[j+3] = 0;						
							a[j+4] = 0;
							a[j+5] = 0x51;		/* push %ecx */	 
							j += 6;
						}
					}	
		else
		{
		     if(params[i].tipo == PTR_PAR)
	  	     {
	      		if(params[i].amarrado == 0)
	      		{		 
	         	   a[j] = 0x8b;
	         	   a[j+1] = 0x4d;
		 			a[j+2] = pilha_rec; /* descobrir esse valor: mov ?(%ebp),%ecx */
					pilha_rec -= 4;		
	         	   a[j+3] = 0x51;	/* push %ecx */
		 			j += 4;
	      		}
	      	    else	/* params[i].amarrado == 1 */
	      	    {
		 			ptr_a = (int)params[i].valor.v_ptr;
		 			a[j] = 0xb9;		/* mov $0,%ecx : b9 00 00 00 00 */
		 			auxp = ptr_a << 24;	/* o valor amarrado em little-endian */
		 			auxp >>= 24;
	         	    a[j+1] = (unsigned char)auxp;
		 			auxp = ptr_a << 16;
		 			auxp >>= 24;
	         	    a[j+2] = (unsigned char)auxp;
		 			auxp = ptr_a << 8;
		 			auxp >>= 24;
	         	    a[j+3] = (unsigned char)auxp;
		 			auxp = ptr_a >> 24;
		 			a[j+4] = (unsigned char)auxp;
		 			a[j+5] = 0x51;		/* push %ecx */	 
	         	    j += 6;
	      	      }
		    }
	 	}
	}
	a[j] = 0xe8;
	j++;
	/* Colocar o endereço na instrução call */
	res = (int)f - (int)&a[j+4];	    /* &a[j+4] é a instrução seguinte a call */
	temp = res;
	aux = temp<<24;
	aux >>= 24;	
	a[j] = aux;
	temp = res;
	aux = temp<<16;
	aux >>= 24;	
	a[j+1] = aux;
	temp = res;
	aux = temp<<8;
	aux >>= 24;	
	a[j+2] = aux;
	temp = res;
	aux = temp>>24;	
	a[j+3] = aux;	
	j += 4;	
	
	for(i=0;i < 4;j++,i++)        /* final de toda função em assembly */
	{
	   a[j] = fim[i];
	}
	
	return a;
}

void libera_func (void* func)
{	
	free(func);
}
