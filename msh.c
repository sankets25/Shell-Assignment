    /*NAME: Sanket Manik Salunke
      Student ID: 1001764897 */

    #define _GNU_SOURCE
    #include <stdio.h>
    #include <unistd.h>
    #include <sys/wait.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <string.h>
    #include <signal.h>

    #define WHITESPACE " \t\n"      // We want to split our command line up into tokens
    // so we need to define what delimits our tokens.
    // In this case  white space
    // will separate the tokens on our command line
    #define MAX_COMMAND_SIZE 255    // The maximum command-line size
    #define MAX_NUM_ARGUMENTS 10     // Mav shell only supports fifteen arguments


    static void handle_signal (int sig )
    {

        /*
         Determine which of the two signals were caught and
         print an appropriate message.
        */

        switch( sig )
        {
        case SIGINT:
            // printf("Caught a SIGINT\n");
            break;

        case SIGTSTP:
            //  printf("Caught a SIGTSTP\n");
            break;

        default:
            //  printf("Unable to determine the signal\n");
            break;
        }

    }



    int main()
    {

    //  signal(SIGINT, sigint_handler);
    //  signal(SIGTSTP, sigint_handler);


        struct sigaction act;

        /*
          Zero out the sigaction struct
        */
        memset (&act, '\0', sizeof(act));

        /*
          Set the handler to use the function handle_signal()
        */
        act.sa_handler = &handle_signal;

        /*
          Install the handler for SIGINT and SIGTSTP and check the
          return value.
          */
        if (sigaction(SIGINT, &act, NULL) < 0)
        {
            perror ("sigaction: ");
            return 1;
        }

        if (sigaction(SIGTSTP, &act, NULL) < 0)
        {
            perror ("sigaction: ");
            return 1;
        }


        char history[15][255];
        int history_index=0;
        char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
        int i;
        char *working_str ;
        int child_pid;

        for( i = 0; i < 15; i++ )
        {
            memset( history[i], 0, 255 );
        }


        int listpid[15];
        int listpid_index=0;
        int j;

        for( j = 0; j < 15; j++ )
        {
            listpid [j] =0;
        }

        while( 1 )
        {


            // Print out the msh prompt
            printf ("msh> ");

            // Read the command from the commandline.  The
            // maximum command that will be read is MAX_COMMAND_SIZE
            // This while command will wait here until the user
            // inputs something since fgets returns NULL when there
            // is no input
            while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

            /* Parse input */
            char *token[MAX_NUM_ARGUMENTS];

            int   token_count = 0;
            // Pointer to point to the token
            // parsed by strsep
            char *arg_ptr;

            //char *working_str  = strdup( cmd_str );
            if( cmd_str[0] == '!')
            {
                working_str  = strdup( cmd_str +1);
                strcpy( working_str, history[atoi(working_str)]);
            }
            else
            {
                working_str  = strdup( cmd_str );
            }




            // we are going to move the working_str pointer so
            // keep track of its original value so we can deallocate
            // the correct amount at the end

            // Tokenize the input stringswith whitespace used as the delimiter
            while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
                    (token_count<MAX_NUM_ARGUMENTS))
            {
                token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
                if( strlen( token[token_count] ) == 0 )
                {
                    token[token_count] = NULL;
                }
                token_count++;
            }

            strcpy( history[history_index++],cmd_str);
            if (history_index>14)
            {
                history_index=0;
            }


            if (listpid_index>14)
            {
                listpid_index=0;
            }


            // }

            // Now print the tokenized input as a debug check
            if( token[0] == 0 )
                continue;




            if (strcmp ("exit",token[0])==0 || strcmp (token[0], "quit")==0)

            {
                exit(0);
            }
            else if (strcmp (token[0], "cd")==0)

            {
                chdir(token[1]);
            }
            else if (strcmp (token[0], "bg")==0)
            {
                kill(child_pid, SIGCONT);
            }
            else if( strcmp("history", token[0] ) == 0 )
            {
                int j = 0;
                int i;
                int index = history_index;
                for( i = 0; i < 15; i++ )    ///15--14
                {
                    if( history[index][0] != 0 )      //changes in 2D array

                        printf("%d: %s", j++, history[index] );   //i--j
                    index++;

                    if ( index > 14 )
                        index = 0;

                }
            }


            else if( strcmp("listpid", token[0] ) == 0 )
            {

                int i;
                int index = listpid_index;
                for( i = 0; i < 15; i++ )    ///15--14
                {
                    if( listpid[i]!= 0 )

                        printf("%d: %d \n", i, listpid[i] );
                    index++;

                    if ( index > 14 )
                        index = 0;

                }
            }




            else
            {
                child_pid = fork();
                if( child_pid == 0 )
                {
                    if(execvp(token[0],  token ) == -1 )
                    {
                        printf("Command not found \n");
                    }
                    exit( 0 );
                }
                else
                {
                    int status;
                    listpid[listpid_index++] = child_pid;
                    waitpid( child_pid, &status, 0 );
                }
            }


        }
    return 0;
    }
