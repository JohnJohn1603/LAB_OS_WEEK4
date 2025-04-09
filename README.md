# LAB_OS_WEEK4
Trần Gia Kiệt - Nguyễn Huy Lượng
struct queue_t *runninglist = caller->running_list;
    int take_idx = -1;
    for (int i = 0; i < runninglist->size; i++){
        struct pcb_t *take_proc = runninglist->proc[i];
        if (strstr(take_proc->path, proc_name) == 0){
            take_idx = i;
            break;
        }
    }
    if (take_idx != -1){
        struct pcb_t *take_proc = runninglist->proc[take_idx];
        for(int i = take_idx + 1; i < runninglist->size; i++){
            runninglist->proc[i-1] = runninglist->proc[i];
        }
        free(take_proc);
        take_proc = NULL;
        runninglist->size--;
    }
