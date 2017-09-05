static int test_ptg_tamanho() {
    /* TODO: implementar! */
    return 0;
}

static int test_ptg_agora_rfc2822() {
    /* TODO: implementar! */
    return 0;
}

int main() {
    unsigned int error_count = 0;
    error_count += test_ptg_tamanho();
    error_count += test_ptg_agora_rfc2822();
    return error_count;
}