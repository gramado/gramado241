/*
 * File: ps/disp/x86start.c
 *
 * Descri??o:
 *     Rotinas de execu??o de thread e processos.
 *     Part of the dispatcher module.
 * 
 * History:
 *     2015 - Created by Fred Nora.
 */


#include <kernel.h>


//
// Fun??es importadas.
//

extern void turn_task_switch_on (void);


/*
 ****************************************************************
 * start_task: 
 *     Habilita uma tarefa para ser executada pela primeira vez, 
 * alertando sobre a presen?a dela.
 *
 * @todo: 
 *     Poderia ser um kernel request?!
 *     #esse nome precisa ser resolvido ... n?o usaremos o termo task.
 */

void 
start_task ( 
    unsigned long id, 
    unsigned long *task_address )
{

    // #todo: 
    // Filtrar argumentos.

    //if (id<0)
    //    panic("start_task: id\n");

	//  start_new_task_status:
	//      ? uma FLAG. Indica que existe uma thread querendo ser executada 
	//  pela primeira vez.
	// #todo: 
	//  Isso poderia estar na estrutura 'ProcessorBlock' ?!

    start_new_task_status = 1; 

    // Id e Address.
    start_new_task_id      = (int) id; 
    start_new_task_address = (unsigned long) task_address;

    // mais ??!!
}




/*
 * switch_to_user_mode:  
 */

void switch_to_user_mode (void)
{
    panic ("switch_to_user_mode:");    //suspensa.
}



// #todo
// Deletar isso

unsigned long 
executa_tarefa ( 
    int id, 
    unsigned long *task_address )
{

	// OBS: 
	// A thread a ser executada, precisa ser a current.   

    struct thread_d *t;


    if ( current_thread != id )
    {
        printf ("executa_tarefa: current_thread = %d | new task = %d ",
            current_thread, id );
        die();
    }

    // Structure.

    t = (void *) threadList[id];

    if ( (void *) t == NULL ){
        panic ("x86start-executa_tarefa: t\n");

    }else{

        // Status.	
	    if( t->state != READY ){
	        panic ("x86start-executa_tarefa: state", id ); 
		}else{
	        t->state = RUNNING;	
	    };
	    //...
    };

	// Running tasks 
	// Incrementa o n?mero de tarefas que est?o rodando.
	
    //ProcessorBlock.threads_counter++;   
    UPProcessorBlock.threads_counter++;   

    IncrementDispatcherCount(SELECT_ANY_COUNT);

    // verbose.
    // printf("executa_tarefa: Executando %d \n",current_thread);
    // refresh_screen();

    // Stack frame.
    unsigned long  eflags = 0x3200;
    unsigned short cs     = 0x1B; 
    unsigned long  eip    = (unsigned long ) task_address;


	// ?? #bugbug ?? rever ?? ? esse mesmo o seletor que queremos para ring3 ???
	//segmento de dados.

	asm (" movw $0x0010, %ax ");
	asm (" movw %ax, %ds ");
	asm (" movw %ax, %es ");
	asm (" movw %ax, %fs ");
	asm (" movw %ax, %gs ");


	// #bugbug
	// Os carinhas do gcc 9.1.0 resolveram sacanear.
	// N?o posso usar mais isso porque eles tiveram um ataque de pelanca.
	// Warning:
	// "Listening the stack pointer register in a clobber list is deprecated." 	
	
    /*
    asm (" pushl %0" :: "r" (eflags): "%esp");  
    asm (" pushw %0" :: "r" (cs)    : "%esp");             
    asm (" pushl %0" :: "r" (eip)   : "%esp");  
    */
    
    asm (" pushl %0" :: "r" (eflags) );  
    asm (" pushw %0" :: "r" (cs)     );             
    asm (" pushl %0" :: "r" (eip)    ); 


    // #bugbug
    // Em que momento essa fun?ao foi chamada.
    // Se ouve uma interrupcao de timer entao ...

    // #bugbug
    // EOI, only for the first pic ?

    out8 ( 0x20, 0x20 ); 

    asm ("sti");
    asm ("iret");

	//Nothing.

    panic ("x86start-executa_tarefa: *");
}


void KiSwitchToUserMode (void)
{
    switch_to_user_mode();
}


// #todo: Delete this thing.
unsigned long KiExecutaTarefa ( int id, unsigned long *task_address )
{
    return 0;  //Deprecated
}


int KiInitTask (int id)
{
    if (id < 0){
        return (int) 0;
    }

    return (int) init_task (id);
}

void KiInitTasks (void)
{
    init_tasks();
}


//
// End.
//

