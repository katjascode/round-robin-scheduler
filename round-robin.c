#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

#define NULL ((void *)0)

#define MAX_TASKS 4

void setup_timer1();
void blink1 (void);
void blink2 (void);
void blink3 (void);
void blink4 (void);
void add_task (uint8_t id, void (*ptr) (void), uint8_t brst);

uint8_t taken = 0;

struct task
{
    uint8_t tid;             /* task id*/
    void (*fct_ptr) (void);  /* blink LED function pointer */
    uint8_t burst_t;         /* task burst time */
    uint8_t rest_t;          /* how much time units are left till completion */
    struct task *task_nxt;   /* pointer to the next task in the queue */
} task_queue[MAX_TASKS];

struct task *head = NULL;
struct task *tail = NULL;

int main(void)
{
    DDRB |= _BV(DDB5) | _BV(DDB0);     // initialize port B, configure 5th bit of DDRB as output
    PORTB &= ~_BV(PORTB5) & ~_BV(PORTB0); //turn off internal LED on pin 13 connected to PORTB 5

    DDRD |= _BV(DDD4) | _BV(DDD2);
    PORTD &= ~_BV(PORTD4) & ~_BV(PORTD2);
    add_task(1, &blink1, 2);
    add_task(2, &blink2, 4);
    add_task(3, &blink3, 4);
    add_task(4, &blink4, 3);

    setup_timer1();
    (head->fct_ptr)();
    sei();
    while(1);
    return 0;
}

ISR(TIMER1_COMPA_vect)
{
    if (head == NULL) return;
    (head->fct_ptr)();
    head->rest_t--;

    if (head->rest_t == 0)
    {
        head = head->task_nxt;
    }
    
    else if (head->task_nxt != NULL)
    {
        tail->task_nxt = head;
        tail = head;
        head = head->task_nxt;
        tail->task_nxt = NULL;
    }
    if (head != NULL)
        (head->fct_ptr)();
    else
    {
        blink1(); blink2(); blink3(); blink4(); taken = 0;
    }
    sei();
}
void setup_timer1() /* initialize timer1 with the period of 500 ms */
{
    // set up timer with prescaler = 1024 and CTC mode
    TCCR1B |= (1 << WGM12)|(1 << CS12)|(1 << CS10);
  
    TIMSK1 |= _BV(OCIE1A);  
    // initialize the compare value
    OCR1A = 7812;
}

void blink1 (void)
{
    PORTB ^= _BV(PORTB5);
}


void blink2 (void)
{
    PORTB ^= _BV(PORTB0);
}


void blink3 (void)
{
    PORTD ^= _BV(PORTD4);
}

void blink4 (void)
{
    PORTD ^= _BV(PORTD2);
}

void add_task (uint8_t id, void (*ptr) (void), uint8_t brst)
{
    if (taken >= MAX_TASKS) return;
    task_queue[taken].tid = id;
    task_queue[taken].fct_ptr = ptr;
    task_queue[taken].burst_t = brst;
    task_queue[taken].rest_t = brst;
    task_queue[taken].task_nxt = NULL;
    if (taken == 0) head = &task_queue[taken];

    tail = &task_queue[taken];

    if (taken > 0) task_queue[taken-1].task_nxt = &task_queue[taken];

    taken++;
}
