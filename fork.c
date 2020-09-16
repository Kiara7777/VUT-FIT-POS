/*
 * Projekt: Projekt 1 do predmetu POS: 3 procesy + exit status
 * Autor: Sara Skutova, xskuto00@stud.fit.vutbr.cz
 * Soubor: fork.c
 * Rok: 2017/2018
 */

#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1 /* XPG 4.2 - needed for WCOREDUMP() */

void printInfo(char *identita)
{
    printf("%s identification: \n", identita); /*grandparent/parent/child */
    printf("	pid = %d,	ppid = %d,	pgrp = %d\n", getpid(), getppid(), getpgrp()); // cislo procesu, cislo rodicovskeho procesu, cislo skupiny
    printf("	uid = %d,	gid = %d\n", getuid(), getgid()); //cislo skutecneho majitele, cislo skutecne skupiny 
    printf("	euid = %d,	egid = %d\n", geteuid(), getegid()); // cicslo efektivniho majitele, cislo efektivni skupiny
    
}

void printEndInfo(char *identita, pid_t pid, int status)
{
    
    printf("%s exit (pid = %d):", identita, pid); /* and one line from */
    
    if (WIFEXITED(status)) //normalni ukonceni, spojene s WEXITSTATUS
    {
        printf("	normal termination (exit code = %d)\n", WEXITSTATUS(status)); /* or */ 
    }
    else if (WIFSIGNALED(status)) //ukonceni signalem, spojene s WTERMSIG patri tam i  WCOREDUMP
    {      
#ifdef WCOREDUMP
        if (WCOREDUMP(status))
        {
            printf("	signal termination %s(signal = %d)\n", "with core dump ", WTERMSIG (status));
        }
        else
        {
            printf("	signal termination %s(signal = %d)\n", "", WTERMSIG (status)); /* or */
        }  
#else
        printf("	signal termination %s(signal = %d)\n", "", WTERMSIG (status)); /* or */
#endif    
    }
    else
    {
        printf("	unknown type of termination\n");
    }
}

/* ARGSUSED */
int main(int argc, char *argv[])
{
    //kontrola parametru
    if (argc < 2)
    {
        printf("Nedostatecny pocet paramentru, musi byt minimalne 2\n");
        return 1;
    }
    
    int status;
    
    printInfo("grandparent");
    
    pid_t parentValue = fork();
    
    if (parentValue == 0) // parent
    {
        printInfo("parent");
        
        pid_t childValue = fork();
        if (childValue == 0) // child
        {   
            printInfo("child");
            //https://support.sas.com/documentation/onlinedoc/sasc/doc/lr2/execv.htm
            char *execvArgv[argc]; //musi tam byt dost mista, na konci je NULL
            
            for(int i = 0; i < argc - 1; i++) // ten posledni ne, musi se tam dat NULL
            {
                execvArgv[i] = argv[i + 1];
            }
            execvArgv[argc - 1] = NULL;
            
            if (execv(argv[1], execvArgv) == -1) //-1 chyba neuspech exevc
            {
                perror("execv() error");
                return 1; 
            }  
        }
        else if (childValue > 0) //parent
        {
            waitpid(childValue, &status, 0); // cekam na child
            printEndInfo("child", childValue, status);
        }
        else
        {
            perror("fork() error");
            return 1;    
        }
    }
    else if (parentValue > 0) //grandparent
    {
        waitpid(parentValue, &status, 0); // cekam na parent
        printEndInfo("parent", parentValue, status);
    }
    else
    {
        perror("fork() error");
        return 1;
    }
    
    return 0;
}

