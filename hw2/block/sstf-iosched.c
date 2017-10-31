/*
 * elevator clook
 * Changes made to noop-iosched.c to reflect CLOOK Algorithm 
 * by Courtney Bonn & Isaac Chan
 * CS 444 Fall 2017
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

int disk_pos = -1;

struct clook_data {
	struct list_head queue;
};

static void clook_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
	printk("CLOOK merge\n");
}

static int clook_dispatch(struct request_queue *q, int force)
{
	struct clook_data *nd = q->elevator->elevator_data;
	char rw;
//	struct request *rq;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
	
//	rq = list_first_entry_or_null(&nd->queue, struct request, queuelist);
//	if(rq){	
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		
		disk_pos = blk_rq_pos(rq);	
		if(rq_data_dir(rq) == READ)
			rw = 'R';
		else
		   	rw = 'W';
		printk("CLOOK dispatch %c at %lx\n", rw, blk_rq_pos(rw));
		return 1;
	}
	return 0;
}

//check the sector pos with rq_end_sector
static void clook_add_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;
	struct list_head *h = NULL;
	char rw;


	list_for_each(h, &nd->queue){
	   	//current pos
		  struct request *hc = list_entry(h, struct request, queuelist);
		//if the request position is greater than the disk position
		if(blk_rq_pos(rq) > disk_pos){
		   	//if the request pos < disk pos OR request pos < current pos -> break and add to current pos
		   	if(blk_rq_pos(hc) < disk_pos || blk_rq_pos(rq) < blk_rq_pos(hc))
		   		break;
		} else{
		   	//if the request pos < disk pos AND request pos < current pos -> break and add to current pos
			if(blk_rq_pos(hc) < disk_pos && blk_rq_pos(rq) < blk_rq_pos(hc))
			   	break;
		}
	}
		
	
	//add after current sector position
	list_add_tail(&rq->queuelist, h);

	if(rq_data_dir(rq) == READ)
	   	rw = 'R';
	else
	   	rw = 'W';

	printk("CLOOK add %c at %lx\n", rw, blk_rq_pos(rq));
}

static struct request *
clook_former_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;
	printk("CLOOK former req\n");

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
//	return list_prev_entry(rq, queuelist);
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}


static struct request *
clook_latter_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;
	printk("CLOOK latter req\n");

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}


static int clook_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct clook_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);

	printk("CLOOK init queue\n");
	return 0;
}

static void clook_exit_queue(struct elevator_queue *e)
{
	struct clook_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);

	printk("CLOOK exit queue\n");
}

static struct elevator_type elevator_clook = {
	.ops = {
		.elevator_merge_req_fn		= clook_merged_requests,
		.elevator_dispatch_fn		= clook_dispatch,
		.elevator_add_req_fn		= clook_add_request,
		.elevator_former_req_fn		= clook_former_request,
		.elevator_latter_req_fn		= clook_latter_request,
		.elevator_init_fn		= clook_init_queue,
		.elevator_exit_fn		= clook_exit_queue,
	},
	.elevator_name = "clook",
	.elevator_owner = THIS_MODULE,
};

static int __init clook_init(void)
{
	return elv_register(&elevator_clook);
}

static void __exit clook_exit(void)
{
	elv_unregister(&elevator_clook);
}

module_init(clook_init);
module_exit(clook_exit);


MODULE_AUTHOR("Isaac Chan & Courtney Bonn");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CLOOK IO scheduler");
