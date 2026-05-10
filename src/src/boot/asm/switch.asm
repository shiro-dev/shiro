; switch.asm — cooperative context switch for Shiro kernel tasks.
;
;   void switch_task(uint32_t *old_esp_save, uint32_t new_esp);
;
; Saves the current task's callee-saved registers on its own stack, stores
; that ESP into `*old_esp_save`, swaps to `new_esp`, and pops a matching
; callee-saved frame off the destination stack. The implicit `ret` at the
; end then returns to wherever the destination task was last yielding from
; — or to its entry point, on the very first switch into a freshly spawned
; task (its stack was preformatted to look like a yield from
; switch_task ↘ entry_fn).

global switch_task

switch_task:
    ; Standard cdecl: ret addr at [esp+0], arg0 at [esp+4], arg1 at [esp+8].
    push ebp
    push ebx
    push esi
    push edi
    ; Now: ret_addr=[esp+16], old_esp_save=[esp+20], new_esp=[esp+24]

    mov eax, [esp + 20]    ; eax = &old->esp
    mov [eax], esp          ; *old_esp_save = current ESP

    mov esp, [esp + 24]    ; ESP = new task's saved ESP

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
