/*
 ============================================================================
 Name        : cocuk.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>


typedef struct _matris{
	int satir_boyu;
	int sutun_boyu;
	char satir[500][500];
	char matris_name[5];
}matris;
matris *matris_pool;
int matris_tut=0;
int size_matris,gidecek_pipe;
int sutun_miktari;
int tut=0,tut2=0;

void *multiple(void *t)
{
		int array[500][500];
		int array2[500][500];
		int result_array[500][500];
		int i=0,j=0,k;
		int result,temp=0;
		char hold[10];
		char sonuc_str[2000];
		char giden_str[1000000];
		//matris sonuc;
		char * pch;
		//char * pch2;
		strcpy(giden_str,"");
		for(;tut<size_matris;tut++)
		{
			//printf("xxxxxxxxxxxxxxxxxxxxxxxxxmultiple..%s and %s\n",matris_pool[0].satir[tut],matris_pool[1].satir[tut]);
			pch = strtok (matris_pool[0].satir[tut]," ,");
			//printf("control..%d\n",sutun_miktari);
			    for(k=0;k<sutun_miktari;k++)
			    {
			      //printf ("%s\n",pch);

			      if(pch!=NULL)
			      {
			    	  array[i][j]=atoi(pch);
			    	  j++;
			      }
			      pch = strtok (NULL, " ,");
			    }
			    j=0;
			    i++;
		}
		i=0;
		j=0;
		/*printf("array:\n");
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				printf("%d-",array[i][j]);
			}
			printf("\n");
		}*/

		for(;tut2<size_matris;tut2++)
		{

			pch = strtok (matris_pool[1].satir[tut2]," ,");
			for(k=0;k<sutun_miktari;k++)
			{
				//printf ("%s\n",pch);
				if(pch!=NULL)
				{
					array2[i][j]=atoi(pch);
					j++;
				}
				pch = strtok (NULL, " ,");
			}
			j=0;
			i++;

		}

		//printf("matris yaziliyor..\n");
		for(i=0;i<size_matris;i++){
			for(j=0;j<sutun_miktari;j++)
			{
				result=array[i][j]+array2[i][j];
				result_array[i][j]=result;
				temp=temp+result;
				//printf("%d-",result_array[i][j]);
			}

		}
		if(temp!=0)
		{
			strcpy(sonuc_str,"");
			for(i=0;i<size_matris;i++)
			{
				for(j=0;j<sutun_miktari;j++)
				{
					sprintf(hold,"%d",result_array[i][j]);
					strcat(sonuc_str,hold);
					if(j!=sutun_miktari-1)
						strcat(sonuc_str," , ");
				}

				strcat(giden_str,sonuc_str);
				//printf("SONUC STRINGI:%s\n",giden_str);
				strcat(giden_str,"-");
				strcpy(sonuc_str,"");
			}
		//	printf("pipe a yazıldı....%s\n",giden_str);
			write(gidecek_pipe,giden_str,strlen(giden_str)+1);

		}
		pthread_exit(NULL);
		//printf("process bitti...\n");
		return NULL;
}


int main(argc,argv)


	int argc;
	char *argv[];
	{

		char readbuffer[100000];
		char servis_name[10];
		int fd;
		int gidenpipe;
		int satir_sayisi;
		int sutun_sayisi;
		int boy;
		int core;
		int i=0,j=0,t=0;

		matris matris1;
		fd=atoi(argv[1]);
		gidenpipe=atoi(argv[2]);
		gidecek_pipe=gidenpipe;
		boy=atoi(argv[3]);
		satir_sayisi=atoi(argv[4]);
		size_matris=satir_sayisi;
		core=atoi(argv[5]);
		core=1;
		strcpy(servis_name,argv[6]);
		sutun_sayisi=atoi(argv[7]);
		sutun_miktari=sutun_sayisi;
		pthread_t threadcore;
		char * pch;

			read(fd , readbuffer, 2*satir_sayisi*boy);
			//printf("%s deneme okudu=********%s\n",servis_name,readbuffer);
			//printf("okudugum : \n%s\n",readbuffer);
			pch = strtok (readbuffer,"-\n");

			while (i!=satir_sayisi)
			{
				strcpy(matris1.satir[i],readbuffer);
				matris_pool=realloc(matris_pool,2*sizeof(matris1));
			    strcpy(matris_pool[0].satir[i],pch);
			    pch = strtok (NULL, "-\n");
			    i++;

			}
			i=0;
			//strcpy(readbuffer,"");
			//core=1;
			//read(fd , readbuffer,500);
			//printf("%s okudu=********%s\n",servis_name,readbuffer);
			//pch = strtok (readbuffer,"-\n");
			while (pch != NULL)
			{
				//printf ("matris2 : %s\n",pch);
				strcpy(matris_pool[1].satir[i],pch);
			    pch = strtok (NULL, "-\n");
			    i++;
			}


		//printf("bozuk\n");

		for(j=0;j<core;j++)
		{
			pthread_create(&threadcore,NULL,multiple,(void *)t);
		}

		pthread_join(threadcore,NULL);
		pthread_exit(NULL);
		//printf("process bitti...\n");
	return EXIT_SUCCESS;
}
