#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include "commandes.h"
#include "constante.h"
#include "get_env.h"

void cmd_touch(char ** s_c){

	int argc = 0;
	while(s_c[argc] != NULL){
		//printf("%s\n", s_c[argc]);
		argc++;
	}

	if(argc < 2){
	        printf("Usage : touch <filename>\n");
                return;
        }
                                                                                                                                                                                                  
        FILE *fp = fopen(FILE_INDEX,"r+"); // r+ c'est read and write, si tu met w, c'est read and write mais ça écrase le fichier
        if(fp == NULL){
                printf("Fichier non trouver, ou acces non permis.\n");
        	return;
        }
        int i;
        for(i = 1; i < argc ; i++){
                unsigned long long cursor;
                                                                                                                                                                                                  
                fseek(fp, 0, SEEK_SET);
                fread(&cursor, sizeof(cursor), 1, fp);
                                                                                                                                                                                                  
                fseek(fp, cursor, SEEK_SET);
                                                                                                                                                                                                  
                // valeur a rajouter dans le fichier
                char active = 1; // octet active
                unsigned long long parent; // parent est envoyé en argument de l'executable, pour le moment se sera 0 // ### a changer
                get_parent(&parent);
                char type_file = 1; // type de fichier, 1 = fichier (parce que touch), 0 pour un dossier
                char file_name[255]; // le nom du fichier // ### quand un ficheir est creer avec touch, on n'écris rien dans stockage.jjg, dés qu'il y a du contenue on remplacera cette valeur..
                unsigned long long cursor_stock = 0; // adresse a laquelle se trouve le fichier dans stockage.jjg // ### a changer, aller chercher la valeur dans le fichier stockage.jjg
                unsigned long long length_file = 0; // taille du fichier, 0 puisque juste créer, par de contenue
		unsigned long long machine;
		unsigned long long round_robin;
		get_round_robin(&round_robin); 

		int size;
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		machine = (round_robin % (size-1)) +1;
                                                                                                                                                                                  
                strcpy(file_name, s_c[i]); // on copie la chaine de charactere dans file_name pour être sur d'avoir les 255 char
                                                                                                                                                                                                  
                // on verifie que la personne n'essaye pas de rajouter quelque chose qui existe déjà
                if(file_exist(FICHIER, file_name) != 0){
                        printf("Le fichier : \"%s\" existe déjà.\n", file_name);
                } else {
                                                                                                                                                                                                  
                        // on ajoute les valeurs
			ajouterLigne(cursor, active, parent, type_file, file_name, cursor_stock, length_file, machine);
			/*
                        fwrite(&active,sizeof(active), 1, fp); // on met rend active la ligne
                        fwrite(&parent,sizeof(parent), 1, fp); // on écris le parent ### pas encore fait
                        fwrite(&type_file,sizeof(type_file), 1, fp);
                        fwrite(file_name,sizeof(file_name), 1, fp);
                        fwrite(&cursor_stock,sizeof(cursor_stock), 1, fp);
                        fwrite(&length_file,sizeof(length_file), 1, fp);
                        */                                                                                                                                                                        
                        // on remet le cuseur de debut de fichier de index.jjg a la nouvelle valeur
                        cursor += INDEX_LINE_SIZE;
                        fseek(fp, 0, SEEK_SET); // on se remet au début
                        fwrite(&cursor, sizeof(cursor), 1, fp);
			
			set_round_robin(round_robin+1);
                }
        }
        fclose(fp);

}

void cmd_deldir(char ** s_c){

	int argc = 0;
	while(s_c[argc] != NULL){
		//printf("%s\n", s_c[argc]);
		argc++;
	}
	
	if(argc < 2){
			printf("Usage : deldir <foldername>\n");
			return ;
	}

	unsigned long long parent;
	get_parent(&parent);

	char file_name[255];
	strcpy(file_name, s_c[1]);

	unsigned long long tmp_cursor = file_exist(DOSSIER, file_name);

	if(tmp_cursor != 0){ // le fichier existe
	supprimerDossier(file_name, tmp_cursor);
	} else {
			printf("Le dossier : %s, n'existe pas.\n", file_name);
	}
}

void cmd_del(char ** s_c){

	int argc = 0;
	while(s_c[argc] != NULL){
		//printf("%s\n", s_c[argc]);
		argc++;
	}

	if(argc < 2){
			printf("Usage : del <filename>\n");
			return ;
	}

	FILE *fp = fopen(FILE_INDEX, "r+");
	if(fp == NULL){
			printf("Fichier non trouver, ou acces non permis.\n");
			return ;
	}

	char file_name[255];
	strcpy(file_name, s_c[1]);

	char active = 0;

	unsigned long long tmp_cursor = file_exist(FICHIER, file_name);

	if(tmp_cursor != 0){ // le fichier existe
			fseek(fp, tmp_cursor, SEEK_SET); // je vais sur la ligne du fichier
			fwrite(&active, sizeof(active), 1, fp); //je met le char active a 0.
	} else {
			printf("Le fichier : %s, n'existe pas.\n", file_name);
	}

        fclose(fp);
}

void cmd_vim(char ** s_c){

	int argc = 0;
	while(s_c[argc] != NULL){
		//printf("%s\n", s_c[argc]);
		argc++;
	}

	// usage
	if(argc < 2){
		printf("Usage : vim <filename>\n");
		return;
	}

	FILE *fp = fopen(FILE_INDEX,"r+"); // r+ c'est read and write, si tu met w, c'est read and write mais ça écrase le fichier
	if(fp == NULL){
		printf("Fichier non trouver, ou acces non permis.\n");
		return;
	}

	unsigned long long cursor;		

	char active; // octet active
	unsigned long long parent; // parent est envoyé en argument de l'executable, pour le moment se sera 0 // ### a changer
	get_parent(&parent);
	char file_type;
	char file_name[255]; // le nom du fichier // ### quand un ficheir est creer avec touch, on n'écris rien dans stockage.jjg, dés qu'il y a du contenue on remplacera cette valeur..
	unsigned long long file_cursor_stock;
	unsigned long long file_size;
	unsigned long long machine;

	strcpy(file_name, s_c[1]); // on copie la chaine de charactere dans file_name pour être sur d'avoir les 255 char

	cursor = file_exist(FICHIER, file_name);

	if(cursor !=0){
		fseek(fp, cursor, SEEK_SET); // on place le cursor au bon endroit.
		//On crée le fichier fichier sur le filesystem hote.
		// On recupere les infos.
		fread(&active, sizeof(active), 1, fp);
		fread(&parent, sizeof(parent), 1, fp);
		fread(&file_type, sizeof(file_type), 1, fp);
		fread(file_name, sizeof(file_name), 1, fp);
		fread(&file_cursor_stock, sizeof(file_cursor_stock), 1, fp);
		fread(&file_size, sizeof(file_size), 1, fp);
		fread(&machine, sizeof(machine), 1, fp);
		char concat[4096];
		concat[0] = '\0';
		strcat(concat, PATH_FOLDER_SWAP); // on met le chemin vers le dossier de swap
		strcat(concat, file_name); // on rajoute le nom du fichier
		strcat(concat, ".swp");  // on rajotue l'extension .swp
		// on creer le fichier
		FILE *fp_swp = fopen(concat, "w+"); // on crée le fichier.
		fclose(fp_swp);
		
		// on regarde si le fichier n'est pas vide, on le remplis. // oué oué ça fait sens
		//if(file_cursor_stock != 0 && file_size != 0) {
		extract_file(concat, file_cursor_stock, file_size, machine);
		//}

	//	On ouvre le fichier ainsi crée grace a vim
		char command[4096];
		snprintf(command, sizeof(command), "xterm -e 'vim %s'", concat); 
		system(command);
		//system("ls");

		unsigned long long cursor_stock;
	//	Une fois le fichier éditer, on écrit le résultat dans notre filesystem
		put_file(concat, &file_size, machine, &cursor_stock);

		// on change le fichier dans index, avec la nouvelle taille et l'endroit ou on le met.. ###
		ajouterLigne(cursor, active, parent, file_type, file_name, cursor_stock, file_size, machine);
	
	//	On supprimer le fichier dans le filesystem hote
		remove(concat);
	} else {	
		printf("Le fichier \"%s\" n'existe pas.\n", file_name);
	}
	fclose(fp);
}

void cmd_rename(char ** s_c){

	int argc = 0;
	while(s_c[argc] != NULL){
		//printf("%s\n", s_c[argc]);
		argc++;
	}

	/**
	 *	Argument : rename f|d NomSource Nomdestination
	 *	### ajouter parent
	 **/

	// Usage	
	if(argc < 4){
		printf("Usage : rename <d|f> <name_source> <name_dest>\n");
		return ;
	}

	FILE *fp = fopen(FILE_INDEX,"r+"); // r+ c'est read and write, si tu met w, c'est read and write mais ça écrase le fichier
	if(fp == NULL){
		printf("Fichier non trouver, ou acces non permis.\n");
	}

	// valeur a rajouter dans le fichier
	unsigned long long parent; // parent est envoyé en argument de l'executable, pour le moment se sera 0 // ### a changer
	get_parent(&parent);
	char type_file = 1; // type de fichier, 1 = fichier (parce que touch), 0 pour un dossier 
	char file_name_src[255]; // le nom du fichier // ### quand un ficheir est creer avec touch, on n'écris rien dans stockage.jjg, dés qu'il y a du contenue on remplacera cette valeur..
	char file_name_dest[255];

	strcpy(file_name_src, s_c[2]); // on copie la chaine de charactere dans file_name pour être sur d'avoir les 255 char
	strcpy(file_name_dest, s_c[3]);
	int size = strlen(s_c[3]);
	if(size < 255){
		file_name_dest[size] = '\0';
	}

	if(s_c[1][0] == 'd'){
		type_file = DOSSIER;
	} else if(s_c[1][0] == 'f'){
		type_file = FICHIER;
	} else {
		printf("Sois c'est f, sois c'est d..\n");
		return ;
	}

	// on verifie que la personne n'essaye pas de rajouter quelque chose qui existe déjà
	parent = file_exist(type_file, file_name_src);
	if(parent != 0){ // le fichier existe
		if(file_exist(type_file, file_name_dest) != 0){
			printf("Le fichier %s existe déjà\n", file_name_dest);	
		} else {
			fseek(fp, parent + 1 + 8 + 1, SEEK_SET);	
			fwrite(file_name_dest,sizeof(file_name_dest), 1, fp); 
		}
	} else {	
		printf("le fichier %s n'existe pas.\n", file_name_src);
	}

	fclose(fp);
}

void cmd_pwd(char ** s_c){

	int argc = 0;
	while(s_c[argc] != NULL){
		//printf("%s\n", s_c[argc]);
		argc++;
	}

	unsigned long long parent;
	get_parent(&parent);

	FILE *fp = fopen(FILE_INDEX, "r");
        if(fp == NULL){
        	printf("Fichier non trouver, ou acces non permis.\n");
			return ;
        }

	int finish = 0;
	char path[4096] = "";

	char active;
    	unsigned long long tmp_parent;
    	char type_file;
	char folder_name[255];
	char concat[4096];

	while(!finish){
				
		fread(&active, sizeof(active), 1, fp);
	
		if(parent == 0){
			concat[0] = '/'; concat[1] = '\0';
			strcat(concat, path);
			printf("%s\n", concat);
			finish = 1;
		} else {
			fseek(fp, parent, SEEK_SET);

			fread(&active, sizeof(active), 1, fp);
                        fread(&tmp_parent, sizeof(tmp_parent), 1, fp);
                        fread(&type_file, sizeof(type_file), 1, fp);
                        fread(&folder_name, sizeof(folder_name), 1, fp);

			strcpy(concat, folder_name);
			strcat(concat, "/");
		
			strcat(concat, path);
			strcpy(path, concat);
			
			parent = tmp_parent;
		}
	}
	
	fclose(fp);
}

void cmd_cd(char ** s_c){
	
	int argc = 0;
	while(s_c[argc] != NULL){
		//printf("%s\n", s_c[argc]);
		argc++;
	}

	if(argc < 2){
		printf("Usage : cd <foldername>\n");
		return ;
	}

	FILE *fp = fopen(FILE_INDEX, "r+");
	if(fp == NULL){
		printf("Fichier non trouver, ou acces non permis.\n");
		return ;
	}

	char folder_name[255];
	strcpy(folder_name, s_c[1]);
	unsigned long long parent;

	// on gere le retour au dossier parent
        if(strcmp(folder_name, "..") == 0 || strcmp(folder_name, "../") == 0){
        	get_parent(&parent);// lire le dossier actuel (get_parent) 
        	if(parent != 0){// si 0, rien faire // sinon, aller au pointeur, et lire son parent.
			fseek(fp, parent + 1, SEEK_SET); // on va au bon endroit, le +1 c'est pour l'active
			fread(&parent, sizeof(parent), 1, fp);
			set_parent(parent);// changer la valeur du parent environnement
		}
		fclose(fp);
        	return ;
        }

	unsigned long long tmp_cursor = file_exist(DOSSIER, folder_name);
	
	if(tmp_cursor != 0){ // le dossier existe
		set_parent(tmp_cursor); // on met le parent a jour.
	} else {
		printf("Le dossier : %s, n'existe pas.\n", folder_name);	
	}

	fclose(fp);
}

void cmd_mkdir(char ** s_c){

	int argc = 0;
	while(s_c[argc] != NULL){
		//printf("%s\n", s_c[argc]);
		argc++;
	}

	if(argc < 2){
		printf("Usage : mkdir <foldername>\n");
		return ;
	}

	FILE *fp = fopen(FILE_INDEX,"r+"); // r+ c'est read and write, si tu met w, c'est read and write mais ça écrase le fichier
	if(fp == NULL){
		printf("Fichier non trouver, ou acces non permis.\n");
		return ;
	}

	unsigned long long cursor;

	// valeur a rajouter dans le fichier
	char active = 1; // octet active
	unsigned long long parent; // parent est envoyé en argument de l'executable, pour le moment se sera 0 // ### a changer
	get_parent(&parent);
	char type_file = DOSSIER; // type de fichier, 1 = fichier (parce que touch), 0 pour un dossier 
	char file_name[255]; // le nom du fichier // ### quand un ficheir est creer avec touch, on n'écris rien dans stockage.jjg, dés qu'il y a du contenue on remplacera cette valeur..

	strcpy(file_name, s_c[1]); // on copie la chaine de charactere dans file_name pour être sur d'avoir les 255 char

	// on verifie que ce n'est pas le dossier ".."
	if(strcmp(file_name, "..") == 0){
		printf("Pas de ça chez nous ! nom de dossier \"..\" reserver\n");
		return ;
	}

	if(file_exist(DOSSIER, file_name) != 0){
		printf("Le dossier : \"%s\" existe déjà.\n", file_name);
	} else {
		
		//on met le curseur du fichier au bon endroit
		fseek(fp, 0, SEEK_SET);               		
		fread(&cursor, sizeof(cursor), 1, fp);
		fseek(fp, cursor, SEEK_SET);
		
		// on ajoute les valeurs
		fwrite(&active,sizeof(active), 1, fp); // on met rend active la ligne 
		fwrite(&parent,sizeof(parent), 1, fp); // on écris le parent
		fwrite(&type_file,sizeof(type_file), 1, fp); 
		fwrite(file_name,sizeof(file_name), 1, fp);

		// on remet le cuseur de debut de fichier de index.jjg a la nouvelle valeur
		cursor += INDEX_LINE_SIZE;
		fseek(fp, 0, SEEK_SET); // on se remet au début
		fwrite(&cursor, sizeof(cursor), 1, fp);
	}

	fclose(fp);
}

void cmd_ls(char ** s_c){

	int argc = 0;
       	while(s_c[argc] != NULL){
       		//printf("%s\n", s_c[argc]);
       		argc++;
       	}

	FILE *fp = fopen(FILE_INDEX, "r");
	if(fp == NULL){
		printf("Fichier non trouver, ou acces non permis.\n");
		return;
	}

    	unsigned long long cursor_end;
    	unsigned long long cursor_tmp = FILE_BASE_ENV;
	unsigned long long parent;

	fseek(fp, 0,  SEEK_SET);
	fread(&cursor_end, sizeof(cursor_end), 1, fp);
	get_parent(&parent);

	fseek(fp, cursor_tmp,  SEEK_SET);
	
	char active;
	unsigned long long tmp_parent;
	char type_file;
	char file_name[255];
	char char_type_file;
	unsigned long long file_cursor_stock;
	unsigned long long file_size;
	unsigned long long machine;

	while(cursor_tmp < cursor_end){
		fread(&active, sizeof(active), 1, fp);
		fread(&tmp_parent, sizeof(tmp_parent), 1, fp);
		fread(&type_file, sizeof(type_file), 1, fp);
		if(active && tmp_parent == parent){ // on verifie qu'il est actif, et qu'il est bien dans le repertoire courant
			fread(file_name, sizeof(file_name), 1, fp);
			// on affiche les infos 
			if(type_file == FICHIER){ // fichier
				//fseek(fp, sizeof(unsigned long long), SEEK_CUR); // on saute l'endroit ou est stocké le fichier
				fread(&file_cursor_stock, sizeof(file_cursor_stock), 1, fp);
				fread(&file_size, sizeof(file_size), 1, fp);
				fread(&machine, sizeof(machine), 1, fp);
				char_type_file = 'f';	
				printf("%c\t%s\t%llu\t%llu\n", char_type_file, file_name,file_size, machine); 
				
			} else {
				char_type_file = 'd';
				file_size = 1; // on met file_size a 1...
				printf("%c\t%s\t%llu\n", char_type_file, file_name,file_size); 
			}
		}

		cursor_tmp += INDEX_LINE_SIZE;
		fseek(fp, cursor_tmp, SEEK_SET); // on place le curseur au bon endroit pour la suite ( parce qu'on ne lis pas tout.. surtout si c'est un fichier..

	}

	fclose(fp);

}

void cmd_help(){

	// le printf
	printf("cd : Change de répertoire courant.\n\ndel : Supprime un fichier dans le répertoire courant. \n\ndeldir : Supprime un répertoire dans le répertoire courant ainsi que tous ses fils récursivement. \n\nls : Affiche le contenu du répertoire courant. \n\nmkdir : Crée un répertoire dans le répertoire courant. \n\npwd : Affiche le chemin absolu du répertoire courant. \n\nrename : Renomme un fichier ou un répertoire dans le répertoire courant. \n\ntouch : Crée des fichiers dans le répertoire courant. \n\nvim : Lance l'éditeur de texte vim sur un fichier passé en paramètre. \n\nexit : Quitte le terminal. \n\nhelp : Affiche la liste des commandes. \n");
}

// Commande Touch
/*char* cmd_touch(char* cmd, char nameMach[MPI_MAX_PROCESSOR_NAME]){
	int match;
	char* ret = (char *) malloc(sizeof(char)*1024); // on sait que ca ne dépassera pas 1024
	strcpy(ret,"");
	
	// Vérifie si ya des fichiers
	char *str_regex_hasFich = "^[ ]*touch[ ]*$";
	match=execRegex(str_regex_hasFich,cmd);
	if (match == 0) {	
		strcpy(ret,"vous devez renseigner un fichier"); 
	}
	
	// Vérifie avec regex si la commande est touch
	char *str_regex = "^[ ]*touch[ ]*[ -_[:alnum:]]+$";
	match=execRegex(str_regex,cmd);
	if (match == 0) {
	
		// Récupère le nom du fichier dans la commande
		char *split, *nameFich;
		split=strtok(cmd," "); 
		while(1){
			do{
				split=strtok(NULL," ");
				if (split==NULL) break; // si ya plus de fichiers on quitte
				nameFich=split;
			} while((strcmp(split,"touch")==0 || strcmp(split,"")==0) );
			if (split==NULL) break; // si ya plus de fichiers on quitte
			
			// Mise à jour du fichier commun			
			char chemin[50];
			strcpy(chemin,nameFich);
			if (ajouterLigne(nameMach,chemin)){
				// info retour
				if (!strcmp(ret,""))
					strcpy(ret,"fichier crée\0"); 	
				else
					strcat(ret,"\nfichier crée\0");
			}
			else{
				// info retour
				if (!strcmp(ret,""))
					strcpy(ret,"fichier déja existant\0");
				else
					strcat(ret,"\nfichier déja existant\0");
			}
			
			// TODO: Renseigner le code pour creation du fichier réel
			
		}

	}
	return ret;
} */

// Commande Rm
/*char* cmd_rm(char* cmd){
	int match;
	char* ret = (char *) malloc(sizeof(char)*1024); // on sait que ca ne dépassera pas 1024
	strcpy(ret,"");
	
	// Vérifie si ya des fichiers
	char *str_regex_hasFich = "^[ ]*rm[ ]*$";
	match=execRegex(str_regex_hasFich,cmd);
	if (match == 0) {	
		strcpy(ret,"vous devez renseigner un fichier"); 
	}
	
	// Vérifie avec regex si la commande est touch
	char *str_regex = "[ ]*rm[ ]*[-_[:alnum:]]+";
	match=execRegex(str_regex,cmd);
	if (match == 0) {			
		// Récupère le nom du fichier dans la commande
		char *split, *nameFich;
		split=strtok(cmd," ");	// TODO: faire pour de multiple fichier
		while(1){
			do{
				split=strtok(NULL," ");
				if (split==NULL) break; // si ya plus de fichiers on quitte
				nameFich=split;
			} while((strcmp(split,"touch")==0 || strcmp(split,"")==0) );
			if (split==NULL) break; // si ya plus de fichiers on quitte
			
			// Mise à jour du fichier commun			
			char chemin[50];
			strcpy(chemin,nameFich);
			if (supprimerLigne(chemin)){
				// info retour
				if (!strcmp(ret,""))
					strcpy(ret,"fichier supprimé\0");
				else
					strcat(ret,"\nfichier supprimé\0");
			}
			else{
				// info retour
				if (!strcmp(ret,""))
					strcpy(ret,"fichier inexistant\0");
				else				
					strcat(ret,"\nfichier inexistant\0");
			}
				
			// TODO: Renseigner le code pour suppression du fichier réel
			
		}
	}
	
	return ret;
}*/

// Commande Show data
/*
char* cmd_showdata(char* cmd){
	int match;
	char* ret = (char *) malloc(sizeof(char)*1024); //1024 initialement
	strcpy(ret,"");
	
	// Vérifie avec regex si la commande est show data
	char *str_regex = "[ ]*show data[ ]*";
	match=execRegex(str_regex,cmd);
	if (match == 0) {
		// On recupere toutes les lignes du fichier
		f_Fichier *dataFich = getAllLignes();
		// On met les entetes
		strcpy(ret,"Num\tMachine\tChemin \n");
		// Variable temporaire
		char* retTmp=(char *) malloc(sizeof(char)*1024);
		for (int i = 0 ; i < getNbLignes(); i++){	
			// On realloue dynamiquement la variable temporaire (au cas ou la ligne dépasse 1024)
			int lenghtRetTmp = snprintf( NULL, 0, "%d\t%s\t%s \n",i, dataFich[i].machine, dataFich[i].chemin );
			retTmp=realloc(retTmp , lenghtRetTmp + 1 );
			sprintf( retTmp, "%d\t%s\t%s \n",i, dataFich[i].machine, dataFich[i].chemin);
			
			// On concatene en reallouant dynamiquement la variable temporaire(ligne) à la variable de retour
			int lenghtRet = snprintf( NULL, 0, " %s %s \n",ret, retTmp );
			ret=realloc(ret , lenghtRet + 1 );
			strcat(ret,retTmp);
		}
	}
	return ret;
}
*/
