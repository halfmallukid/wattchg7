/* resource.c - resource manager routines */

/* SimpleScalar(TM) Tool Suite
 * Copyright (C) 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 * All Rights Reserved. 
 * 
 * THIS IS A LEGAL DOCUMENT, BY USING SIMPLESCALAR,
 * YOU ARE AGREEING TO THESE TERMS AND CONDITIONS.
 * 
 * No portion of this work may be used by any commercial entity, or for any
 * commercial purpose, without the prior, written permission of SimpleScalar,
 * LLC (info@simplescalar.com). Nonprofit and noncommercial use is permitted
 * as described below.
 * 
 * 1. SimpleScalar is provided AS IS, with no warranty of any kind, express
 * or implied. The user of the program accepts full responsibility for the
 * application of the program and the use of any results.
 * 
 * 2. Nonprofit and noncommercial use is encouraged. SimpleScalar may be
 * downloaded, compiled, executed, copied, and modified solely for nonprofit,
 * educational, noncommercial research, and noncommercial scholarship
 * purposes provided that this notice in its entirety accompanies all copies.
 * Copies of the modified software can be delivered to persons who use it
 * solely for nonprofit, educational, noncommercial research, and
 * noncommercial scholarship purposes provided that this notice in its
 * entirety accompanies all copies.
 * 
 * 3. ALL COMMERCIAL USE, AND ALL USE BY FOR PROFIT ENTITIES, IS EXPRESSLY
 * PROHIBITED WITHOUT A LICENSE FROM SIMPLESCALAR, LLC (info@simplescalar.com).
 * 
 * 4. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 * 
 * 5. Noncommercial and nonprofit users may distribute copies of SimpleScalar
 * in compiled or executable form as set forth in Section 2, provided that
 * either: (A) it is accompanied by the corresponding machine-readable source
 * code, or (B) it is accompanied by a written offer, with no time limit, to
 * give anyone a machine-readable copy of the corresponding source code in
 * return for reimbursement of the cost of distribution. This written offer
 * must permit verbatim duplication by anyone, or (C) it is distributed by
 * someone who received only the executable form, and is accompanied by a
 * copy of the written offer of source code.
 * 
 * 6. SimpleScalar was developed by Todd M. Austin, Ph.D. The tool suite is
 * currently maintained by SimpleScalar LLC (info@simplescalar.com). US Mail:
 * 2395 Timbercrest Court, Ann Arbor, MI 48105.
 * 
 * Copyright (C) 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "host.h"
#include "misc.h"
#include "resource.h"

extern int vdd_values[2][3]; //can be changed later
extern int desired_num_values[2][3];
extern struct config_rom conf_rom[3];
extern int curr_config_rom_index;

int check_validity(int* bit_field,int index,int max_bits){
	
	int dec_val = 0;
	int i = 0;

	for(i=0;i<max_bits;i++)
	{
		dec_val += dec_val<<2 + bit_field[i];
	}

	if(dec_val!=index)
	return 0;

	return 1;
	
}
int* get_curr_num_config()
{
	return conf_rom[curr_config_rom_index].num_bit_field;
}
int* get_curr_vdd_config()
{
	return conf_rom[curr_config_rom_index].vdd_bit_field;
}
int* get_vdd_config(int index)
{
	return conf_rom[index].vdd_bit_field;
}
int* get_num_config(int index)
{
	return conf_rom[index].num_bit_field;
}
int* get_vdd_config_for_cluster(int index,int cluster_index)
{
	return conf_rom[index].vdd_bit_field[2*cluster_index];
}
int get_num_config_for_cluster(int index,int cluster_index)
{
	return conf_rom[index].num_bit_field[cluster_index];
}

int return_vdd_value(struct config_rom rom_config,int cluster_index)
{
	int bit_val = 0;
	bit_val = rom_config.vdd_bit_field[2*cluster_index];
	bit_val += bit_val<<2 + rom_config.vdd_bit_field[2*cluster_index+1];

	return vdd_values[cluster_index][bit_val];

}

int return_num_bits_value(struct config_rom rom_config,int cluster_index)
{

	int bit_val = 0;
	bit_val = rom_config.num_bit_field[cluster_index];

	return desired_num_values[cluster_index][bit_val];
}

/* update a resource pool USELESS ATM*/
struct res_pool * res_update_pool_add(char *name,struct res_desc *old_pool,struct res_desc add_resource,int ndesc)
{
		int i,j,k,index,ninsts;
		struct res_desc *new_inst_pool;
		struct res_pool *new_res;
		//the pointer that was used to access the pool earlier should be now overwritten with the current pointer since we are re-allocating resources.
		//we do need new memory for a new res_desc and hence need to do a malloc/calloc again..?
		//also while removing resources,  the same should be taken care of that the ones that are deactivated are removed , and not the ones taht are being used
		for(ninsts=0, i=0;i<ndesc;i++)
		{
		 if (old_pool[i].quantity > MAX_INSTS_PER_CLASS)
       		 fatal("too many functional units, increase MAX_INSTS_PER_CLASS");
      	         ninsts += old_pool[i].quantity;
		}

		//update the number 
		ninsts += add_resource.quantity;
  		new_inst_pool = (struct res_desc *)calloc(ninsts+1, sizeof(struct res_desc));
		//leave space for new element
		 if (!new_inst_pool)
   		 fatal("out of virtual memory");

		 // the structure vals (even for busy stateshould be copied from the earlier
		 //old_pool struct
		 //and then overwritten for our new one
		 for (index=0,i=0; i<ndesc; i++) //old ndesc TODO
    		{
			//SREEK ALERT - MAY NOT NEED THIS LOOP AT ALL
			if(old_pool[i].quantity>1)
				printf("WARNING! pool has >1 quantity resource\n");

      			for (j=0; j<old_pool[i].quantity; j++)
			{
	  			new_inst_pool[index] = old_pool[i];
	  			new_inst_pool[index].quantity = old_pool[i].quantity;
	  			new_inst_pool[index].busy = old_pool[i].busy;
	  			for (k=0; k<MAX_RES_CLASSES && new_inst_pool[index].x[k].class; k++)
	    			new_inst_pool[index].x[k].master = &new_inst_pool[index];
	  			index++;
			}
    		}

		printf("updating!");
		fflush(stdout);
		//update the index and add teh new element
		new_inst_pool[index] = add_resource;
		new_inst_pool[index].quantity = add_resource.quantity;
		new_inst_pool[index].busy = add_resource.busy;
		printf("past suspect");
		fflush(stdout);
		for (k=0; k<MAX_RES_CLASSES && new_inst_pool[index].x[k].class; k++)
		{
	    		new_inst_pool[index].x[k].master = &new_inst_pool[index];
		}

		
  		//assert(index == ninsts);

		new_res = (struct res_pool *)calloc(1, sizeof(struct res_pool));
  		if (!new_res)
  		  fatal("out of virtual memory");
 		 new_res->name = name;
 		 new_res->num_resources = ninsts;
 		 new_res->resources = new_inst_pool;

		/* fill in the resource table map - slow to build, but fast to access */
  		for (i=0; i<ninsts; i++)
 		 {
			 printf("updating table!\n");
			 fflush(stdout);
 		     struct res_template *plate;
     			 for (j=0; j<MAX_RES_CLASSES; j++)
			{
			  plate = &new_res->resources[i].x[j];
			  if (plate->class)
			    {
			      assert(plate->class < MAX_RES_CLASSES);
	      		      new_res->table[plate->class][new_res->nents[plate->class]++] = plate;
	  		  }
			  else
	 		   /* all done with this instance */
			    break;
			}
    		}

  return new_res;

		
}
int count_int_alus(struct res_pool *res_pool)
{	

	int int_counter = 0;
	struct res_desc *fu_res = res_pool->resources;
	int num_res = res_pool->num_resources;
	char *fu_string = "integer-ALU";
	int i;
	for(i = 0;i<num_res;i++)
	{
		
		if(!strncmp(fu_res[i].name,fu_string,strlen(fu_string)) && fu_res[i].active == 1 )
			int_counter++;
	}
	return int_counter;
}
int count_mult_div_alus(struct res_pool *res_pool)
{
	int mult_div_counter = 0;
	struct res_desc *fu_res = res_pool->resources;
	int num_res = res_pool->num_resources;
	char *fu_string = "integer-MULT/DIV";
	int i;
	for( i= 0;i<num_res;i++)
	{
		
		if(!strncmp(fu_res[i].name,fu_string,strlen(fu_string))&& fu_res[i].active == 1 )
			mult_div_counter++;
	}

	return mult_div_counter;
}
int count_mem_ports(struct res_pool *res_pool)
{
	int mem_port_counter = 0;
	struct res_desc *fu_res = res_pool->resources;
	int num_res = res_pool->num_resources;
	char *fu_string = "memory-port";
	int i;
	for( i = 0;i<num_res;i++)
	{
		
		if(!strncmp(fu_res[i].name,fu_string,strlen(fu_string))&& fu_res[i].active == 1 )
			mem_port_counter++;
	}

	return mem_port_counter;
}
int count_fp_adders(struct res_pool *res_pool)
{
	int fp_adder_counter = 0 ;
	struct res_desc *fu_res = res_pool->resources;
	int num_res = res_pool->num_resources;
	char *fu_string = "FP-adder";
	int i;
	for(i = 0;i<num_res;i++)
	{
		
		if(!strncmp(fu_res[i].name,fu_string,strlen(fu_string))&& fu_res[i].active == 1 )
			fp_adder_counter++;
	}

	return fp_adder_counter;
}
int count_fp_mul_divs(struct res_pool *res_pool)
{
	int fp_mul_divs_counter = 0 ;
	struct res_desc *fu_res = res_pool->resources;
	int num_res = res_pool->num_resources;
	char *fu_string = "FP-MULT/DIV";
	int i;
	for(i = 0;i<num_res;i++)
	{
		
		if(!strncmp(fu_res[i].name,fu_string,strlen(fu_string))&& fu_res[i].active == 1 )
			fp_mul_divs_counter++;
	}

	return fp_mul_divs_counter;
}
int deactivate_alus_to(const int desired_target,int curr_num,enum fu_type target_type,struct res_pool *fu_pool)
{
	int updated_num = 0;

		updated_num = curr_num;
	//	printf("updated num is = %d and curr_num = %d desired target is %d!!!",updated_num,curr_num,desired_target);
		if(curr_num  <= desired_target )
		{
		//	fflush(stdout);
		//	printf("nothing to deactivate!!!!!");
		//	fflush(stdout);
			return 0;
		}
		else
		{
			updated_num = deactivate_first_free_fu(target_type,curr_num,desired_target,fu_pool);
		//	if(updated_num > desired_target)
		//	{
			//	printf("not all resources were free!!!!");
			//	fflush(stdout);
		//	}
		//	else
		//	{
		//		printf("target acheived!!!!");
		//		fflush(stdout);
		//	}
		}
		
	return updated_num;
}
int deactivate_first_free_fu(enum fu_type fu_type,int curr_num,const int desired_target,struct res_pool* fu_pool)
{
	char fu_string[32];
	memset(fu_string,0,32);
	int num_res = curr_num;
	struct res_desc *fu_res = fu_pool->resources;

	switch(fu_type)
	{
		case INT_ALU:
		strcpy(fu_string,"integer-ALU");
			break;
		case INT_MUL_DIV:
			strcpy(fu_string,"integer-MULT/DIV");
			break;
		case MEM_PORT:
			strcpy(fu_string,"memory-port");
			break;
		case FP_ADD:
			strcpy(fu_string,"FP-adder");
			break;
		case FP_MUL_DIV:
			strcpy(fu_string,"FP-MULT/DIV");
			break;
		default:
			strcpy(fu_string,"integer-ALU");
	}	
	int i;
	for(i = 0;i<num_res;i++)
	{
	
	if(!strncmp(fu_res[i].name,fu_string,strlen(fu_string)))
	{
		if(fu_res[i].busy == 0 && fu_res[i].active == 1 && curr_num >desired_target)
		{
			fu_res[i].active = 0;	
			curr_num = curr_num - 1;
		}
	}

	}

	return curr_num;

}
struct res_pool *
res_create_pool(char *name, struct res_desc *pool, int ndesc)
{
	//ndesc is the number of resource desc defined in the fu_config
	//structure
	//pool is the pointer to fu_config array
  int i, j, k, index, ninsts;
  struct res_desc *inst_pool;
  struct res_pool *res;

  /* count total instances */
  for (ninsts=0,i=0; i<ndesc; i++)
    {
	    //the ACTUAL number of instances is calculated now 
	    //by adding the quantities
      if (pool[i].quantity > MAX_INSTS_PER_CLASS)
        fatal("too many functional units, increase MAX_INSTS_PER_CLASS");
      ninsts += pool[i].quantity;
    }

  /* allocate the instance table */
  //allocate enough memory for ninst (actual no of instances)
  inst_pool = (struct res_desc *)calloc(ninsts, sizeof(struct res_desc));
  if (!inst_pool)
    fatal("out of virtual memory");

  /* fill in the instance table */
  for (index=0,i=0; i<ndesc; i++)
    {
      for (j=0; j<pool[i].quantity; j++)
	{
	  inst_pool[index] = pool[i];
	  inst_pool[index].quantity = 1;
	  inst_pool[index].busy = FALSE;
	  //SREEK CHANGES
          inst_pool[index].active = pool[i].active;
	  for (k=0; k<MAX_RES_CLASSES && inst_pool[index].x[k].class; k++)
	    inst_pool[index].x[k].master = &inst_pool[index];
	  index++;
	}
    }
  assert(index == ninsts);

  /* allocate the resouce pool descriptor */
  //res is the resource pool!
  res = (struct res_pool *)calloc(1, sizeof(struct res_pool));
  if (!res)
    fatal("out of virtual memory");
  res->name = name;
  res->num_resources = ninsts;
  res->resources = inst_pool;

  /* fill in the resource table map - slow to build, but fast to access */
  for (i=0; i<ninsts; i++)
    {
      struct res_template *plate;
      for (j=0; j<MAX_RES_CLASSES; j++)
	{
	  plate = &res->resources[i].x[j];
	  if (plate->class)
	    {
	      assert(plate->class < MAX_RES_CLASSES);
	      //this part of the code fills in the entries
	      //for an array , based on teh class value and 
	      //the next slot for the particular class 
	      //and keeps track of it in the res->nents[class] struct
	      res->table[plate->class][res->nents[plate->class]++] = plate;
	    }
	  else
	    /* all done with this instance */
	    break;
	}
    }

  return res;
}

/* get a free resource from resource pool POOL that can execute a
   operation of class CLASS, returns a pointer to the resource template,
   returns NULL, if there are currently no free resources available,
   follow the MASTER link to the master resource descriptor;
   NOTE: caller is responsible for reseting the busy flag in the beginning
   of the cycle when the resource can once again accept a new operation */
struct res_template *
res_get(struct res_pool *pool, int class)
{
  int i;

  /* must be a valid class */
  assert(class < MAX_RES_CLASSES);

  /* must be at least one resource in this class */
  assert(pool->table[class][0]);

  for (i=0; i<MAX_INSTS_PER_CLASS; i++)
    {
      if (pool->table[class][i])
	{
	//SREEK critical change 
	  if (!(pool->table[class][i]->master->busy) 
		&& pool->table[class][i]->master->active)
	    return pool->table[class][i];
	}
      else
	break;
    } 
//SREEK-CHANGES
  if(pool->table[class][i-i])   
   {
 //  printf(" the i value is %d",i);
//   printf(" class is %d\n",class);
 //  printf(" name of hte master is %s",pool->table[class][i-1]->master->name);
   }
  /* none found */
  return NULL;
}

/* dump the resource pool POOL to stream STREAM */
//SREEK updated res_dump function to include active status flag
void
res_dump(struct res_pool *pool, FILE *stream)
{
  int i, j;

  if (!stream)
    stream = stderr;

  fprintf(stream, "Resource pool: %s:\n", pool->name);
  fprintf(stream, "\tcontains %d resource instances\n", pool->num_resources);
  fprintf(stream, "WTF WTF WTF\n");
  
  for (i=0; i<MAX_RES_CLASSES; i++)
    {
      fprintf(stream, "\tclass: %d: %d matching instances\n",
	      i, pool->nents[i]);
      fprintf(stream, "\tmatching: ");
      for (j=0; j<MAX_INSTS_PER_CLASS; j++)
	{
	  if (!pool->table[i][j])
	    break;
	  fprintf(stream, "\t%s (busy for %d cycles) ",
		  pool->table[i][j]->master->name,
		  pool->table[i][j]->master->busy);
          fprintf(stream,"\tactive status is = %d",
		  pool->table[i][j]->master->active);
	}
      assert(j == pool->nents[i]);
      fprintf(stream, "\n");
    }
}
