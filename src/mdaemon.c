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


pthread_mutex_t mutex_matrice_buffer,mutex_result_buffer;
pthread_cond_t work,result,service,client;

int okunan_matris=0,result_okunan_matris=0;
int matricebuffer=0,resultbuffer=0;
int file_empty=0; int buffer_empty=0;
int size_matris;
int result_pool_tut=0,havuz_sira=0;
int matris_sayisi=0;
char client_file[15];
char matris_file[15];
char result_file[15];


typedef struct _matris{
	int satir_boyu;
	int sutun_boyu;
	char satir[500][500];
	char matris_name[50];
}matris;
matris *matris_pool;
matris *matris_result_pool;

typedef struct _computer{
	char computer_name[10];
	int core_number;
}computer;

void * workloader(void * t){
	int m;
	matris m1;
	FILE *dg;        /* dosya işaretçisi */

	if( (dg=fopen(matris_file,"r")) == NULL )
		printf("Dosya açılmadı !\n"), exit(1);

		while( !feof(dg) ) /* dosyanın sonuna kadar */
		{

			fscanf(dg,"%s %d %d",m1.matris_name , &m1.satir_boyu , &m1.sutun_boyu); /* verileri oku! */
			//printf("%s %d %d\n",m1.matris_name , m1.satir_boyu , m1.sutun_boyu);
			if(feof(dg)) break;
			for(m=0;m<m1.satir_boyu+1;m++){//matris satır ve sutun boyuna uygun sekilde okunuyor
				fgets(m1.satir[m],m1.sutun_boyu*4,dg);
			}
			//for(m=1;m<m1.satir_boyu+1;m++){//matris satır ve sutun boyuna uygun sekilde okunuyor
					//printf("%s\n",m1.satir[m]);
			//}
			//printf("\n");

			matris_sayisi++;

				matris_pool=realloc(matris_pool,100*matris_sayisi*200000);
				size_matris=sizeof(m1);

				pthread_mutex_lock(&mutex_matrice_buffer);
				if(matricebuffer>=11)
				{
					//printf("workloader: matrice buffer doldu servis ten sinyal bekleniyor\n");
					pthread_cond_wait(&work,&mutex_matrice_buffer);
					//printf("workloader : servisten sinyal alındı...\n");
					matris_pool[havuz_sira]=m1;
					havuz_sira++;
					//for(i=0;i<matris_sayisi;i++)
						//printf("->->->%s\n",matris_pool[i].matris_name);
					matricebuffer++;
					//printf("workloader: dosyadan bir matrix matrix buffera yüklendi matrice: %d matricebuffer: %d\n",matris_sayisi,matricebuffer);

				}
				else
				{
					matris_pool[havuz_sira]=m1;
					havuz_sira++;
					//for(i=0;i<matris_sayisi;i++)
					//	printf("->->->%s\n",matris_pool[i].matris_name);
					matricebuffer++;
					//printf("workloader: dosyadan bir matrix matrix buffera yüklendi matrice: %d matricebuffer: %d\n",matris_sayisi,matricebuffer);
				}
				pthread_mutex_unlock(&mutex_matrice_buffer);
		}
		fclose(dg);
		file_empty=1;
		//printf("bitiş....");
		return NULL;
}

void * servis(void *threadid){
	computer *my_data;
	my_data = ( computer *) threadid;
	//printf("servis name==========>%d\n",my_data->core_number);
	int i,boy;
	pid_t pid;
	//int a=0;
	int gidenpipe[2];
	int gelenpipe[2];
	char str[5];
	char str2[5];
	char str_satirsayisi[5];
	char str_boy[5];
	char core[5];
	char str_sutunsayisi[5];
	char temp[1000000];
	char name[500];
	char m1[1000000];
	char m2[1000000];
	char *pch;
	//matris_result_pool=malloc(100*sizeof(matris_pool[0]));
	matris gelen_matris;

	while(1){

			pthread_mutex_lock(&mutex_matrice_buffer);
			pthread_mutex_lock(&mutex_result_buffer);
			//matris_result_pool=realloc(matris_result_pool,10*(result_okunan_matris+1)*sizeof(matris_pool[0]));
			matris_result_pool=realloc(matris_result_pool,(result_pool_tut+100)*sizeof(matris_result_pool[result_okunan_matris]));
			if(matricebuffer>=2)
			{
								for(i=0;i<matris_pool[okunan_matris].satir_boyu;i++)
								{
									boy=boy+strlen(matris_pool[okunan_matris].satir[i]);
								}
								for(i=0;i<matris_pool[okunan_matris+1].satir_boyu;i++)
								{
										boy=boy+strlen(matris_pool[okunan_matris+1].satir[i]);
								}
								pipe (gidenpipe);
								pipe (gelenpipe);
								pid = fork ();
								if (pid ==  0)
								{
									close (gidenpipe[1]);
									close (gelenpipe[0]);
									sprintf(str,"%d",gidenpipe[0]);
									sprintf(str2,"%d",gelenpipe[1]);
									sprintf(str_boy,"%d",boy+1);
									sprintf(str_satirsayisi,"%d",matris_pool[okunan_matris].satir_boyu);
									sprintf(core,"%d", my_data->core_number);
									sprintf(str_sutunsayisi,"%d",matris_pool[okunan_matris].sutun_boyu);
									//printf("yazdık...\n");//read_from_pipe (gidenpipe[0]);
									execlp("./client","client",str,str2,str_boy,str_satirsayisi,core,my_data->computer_name,str_sutunsayisi,NULL);
								    return EXIT_SUCCESS;
									//exit(0);
								}
								close (gidenpipe[0]);
								close (gelenpipe[1]);
								//printf("okunana matris:%d\n",okunan_matris);
								strcpy(m1,"");
								strcpy(m2,"");
								for(i=1;i<matris_pool[okunan_matris].satir_boyu+1;i++)
								{
									strcat(m1,matris_pool[okunan_matris].satir[i]);
									//if(i!=matris_pool[okunan_matris].satir_boyu)
										strcat(m1,"-");
									//write(gidenpipe[1],matris_pool[okunan_matris].satir[i],strlen(matris_pool[okunan_matris].satir[i])+1);
									//printf("++++++++++%s gönderildi...\n",matris_pool[okunan_matris].matris_name);
								}
								//printf("%s\n",m1);
								//write(gidenpipe[1],m1,strlen(m1)+1);

								//printf("2.matris.......\n");
								for(i=1;i<matris_pool[okunan_matris+1].satir_boyu+1;i++)
								{
									strcat(m2,matris_pool[okunan_matris+1].satir[i]);
									//if(i!=matris_pool[okunan_matris+1].satir_boyu)
										strcat(m2,"-");
									//write(gidenpipe[1],matris_pool[okunan_matris+1].satir[i],strlen(matris_pool[okunan_matris+1].satir[i])+1);
									//printf("++++++++++%s gönderildi...\n",matris_pool[okunan_matris+1].satir[i]);
								}
								strcat(m1,m2);
								//printf("gonderilen.. \n:%s ve %s\n",matris_pool[okunan_matris].matris_name,matris_pool[okunan_matris+1].matris_name);
								write(gidenpipe[1],m1,strlen(m1)+1);

								strcpy(name,"name");
								//strcat(name,matris_pool[okunan_matris].matris_name);
								//strcat(name,matris_pool[okunan_matris+1].matris_name);
								strcpy(gelen_matris.matris_name,name);
								gelen_matris.satir_boyu=matris_pool[okunan_matris].satir_boyu;
								gelen_matris.sutun_boyu=matris_pool[okunan_matris].sutun_boyu;
								okunan_matris=okunan_matris+2;
								wait(NULL);

									//gelen_matris=malloc(sizeof(matris_pool[0]));

									read(gelenpipe[0],temp,100000 );
									//printf("baba okudu=********\n%s\n",temp);
									pch = strtok (temp,"-\n");
									i=0;
									while (pch != NULL)
									{
										if(i==0)
											strcpy(gelen_matris.satir[i],name);
										strcpy(gelen_matris.satir[i+1],pch);
										//printf ("baba=%s\n",pch);
										pch = strtok (NULL, "-\n");
										i++;
									}


				matricebuffer-=2;
				//printf("servis: matris bufferdan iki eleman işlendi matrice buffer: %d\n",matricebuffer);
				if(matricebuffer < 10  )
				{
					//printf("servis: result a sinyal gönderildi\n");
					pthread_cond_signal(&result);
				}
				if(matricebuffer<19){
					//printf("servis: workloadera sinyal gönderildi\n");
					pthread_cond_signal(&work);
				}


				if(resultbuffer<10)
				{
					matris_result_pool[result_pool_tut]=gelen_matris;
					//printf("astalavista %d\n",result_pool_tut);
					result_pool_tut++;
					resultbuffer++;
					//printf("servis: result buffera yazıldı result:%d\n",resultbuffer);
				}
				else
				{
					//printf("servis: result buffer dolu result dan sinyal bekleniyor\n");
					pthread_mutex_unlock(&mutex_result_buffer);
					pthread_mutex_unlock(&mutex_matrice_buffer);
					pthread_cond_wait(&service,&mutex_result_buffer);
					//printf("servis: result dan sinyal alındı matris result buffera yazıldı\n");
					matris_result_pool[result_pool_tut]=gelen_matris;
					result_pool_tut++;
					resultbuffer++;
				}
			}

			else
			{
				//printf("kritik bolge\n");
				pthread_cond_signal(&result);
				//pthread_cond_signal(&service);

				if(file_empty==1 &&  resultbuffer==0  && matricebuffer==1)
				{
					FILE * ptr;
					if( (ptr=fopen(result_file,"w")) == NULL )
							printf("Dosya açılmadı !\n"), exit(1);
					for(i=1;i<gelen_matris.satir_boyu+1;i++)
					{
						printf("%s\n",gelen_matris.satir[i]);
						fprintf(ptr,"%s\n",gelen_matris.satir[i]);
					}


					printf("işlem tamam!!!");
					exit(1);

				}

				/*if(file_empty==1 && ( resultbuffer!=1 ))//|| resultbuffer!=0 ) )
				{
					pthread_mutex_unlock(&mutex_result_buffer);
					pthread_mutex_unlock(&mutex_matrice_buffer);
					printf("buraya giriyomuki lan...\n");
					pthread_cond_signal(&work);
					pthread_cond_signal(&result);
					pthread_cond_wait(&service,&mutex_matrice_buffer);
				}*/
			}
			pthread_mutex_unlock(&mutex_result_buffer);
			pthread_mutex_unlock(&mutex_matrice_buffer);
		}

	printf("sikko\n");
	return NULL;
}
void * resultloader(void * t){
	//int a=0;
	while(1){
		//sleep(4);
		//printf("result...\n");
		pthread_mutex_lock(&mutex_matrice_buffer);
		pthread_mutex_lock(&mutex_result_buffer);
		if(matricebuffer==20)
		{
			//printf("result: matris buffer dolu servisten sinyal bekle\n");
			pthread_mutex_unlock(&mutex_result_buffer);
			pthread_mutex_unlock(&mutex_matrice_buffer);
			pthread_cond_wait(&result,&mutex_matrice_buffer);
		//	printf("result: servisten sinyal geldi\n");
		}
		if(resultbuffer>0)
		{

			//printf("buraya girmen lazım aq..havuz sıra=%d  result_okunan_matris=%d\n",havuz_sira,result_okunan_matris);
			//printf("aranan matris....%s\n",matris_result_pool[result_okunan_matris].matris_name);
			//printf("aranan matris....%s\n",matris_pool[havuz_sira].matris_name);
			matris_pool=realloc(matris_pool,100*(matris_sayisi+havuz_sira+1)*10000);
			matris_pool[havuz_sira]=matris_result_pool[result_okunan_matris];
			//for(a=0;a<matris_pool[0].satir_boyu;a++)
				//strcpy(matris_pool[havuz_sira].satir[a],matris_result_pool[result_okunan_matris].satir[a]);
			//strcpy(matris_pool[havuz_sira].matris_name,matris_result_pool[result_okunan_matris].matris_name);

			havuz_sira++; 	result_okunan_matris++;
			resultbuffer--;
		//	printf("result: matris resulttan alındı result: %d\n",resultbuffer);
			if(resultbuffer<=9)
			{
				pthread_cond_signal(&service);
				//printf("result: servise sinyal gönderildi\n");
			}
			matricebuffer++;
			//printf("result: matrice buffera yazıldı\n");
		}

		if(matricebuffer>=2)
		{
			pthread_cond_signal(&service);
			//printf("result : servise sinyal attım...\n");
		}
		pthread_mutex_unlock(&mutex_result_buffer);
		pthread_mutex_unlock(&mutex_matrice_buffer);
	}
	//printf("bitto");
	return NULL;
}

void *client_manager(void * t)
{

	pthread_t threadservice;
	computer c1;
	FILE *dg;     /* dosya işaretçisi */
	if( (dg=fopen(client_file,"r")) == NULL )
		printf("Dosya açılmadı !\n"), exit(1);

	while( !feof(dg) ) /* dosyanın sonuna kadar */
	{
		//c1 = (computer)malloc(sizeof(computer));
		fscanf(dg,"%s : %d",c1.computer_name,&c1.core_number); /* verileri oku! */
		if(feof(dg)) break;
		//printf("%s %d\n",c1.computer_name, c1.core_number);
		//printf("servisthread yaratıldı...%d\n",i);
		pthread_create(&threadservice,NULL,servis,(void*)&c1);

	}

	pthread_join(threadservice,NULL);

	return NULL;
}

int main (int argc,char ** argv)
{
	int t=0;
	pthread_t threadwork,threadresult,threadclient;
	pthread_mutex_init(&mutex_matrice_buffer,NULL);
	pthread_mutex_init(&mutex_result_buffer,NULL);

	pthread_cond_init(&work,NULL);
	pthread_cond_init(&result,NULL);
	pthread_cond_init(&service,NULL);
	pthread_cond_init(&client,NULL);

	strcpy(client_file,argv[1]);
	strcpy(matris_file,argv[2]);
	strcpy(result_file,argv[3]);


	pthread_create(&threadresult,NULL,resultloader,(void *)t);
	pthread_create(&threadwork,NULL,workloader,(void *)t);
	pthread_create(&threadclient,NULL,client_manager,(void *)t);

	pthread_join(threadclient,NULL);
	pthread_join(threadresult,NULL);
	pthread_join(threadwork,NULL);
	//printf("program sonlandı");

	pthread_exit(NULL);

	return 0;

}
