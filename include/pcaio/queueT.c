struct queueT_NAME(queue) *
queueT_NAME(queue_create) (unsigned char mask) {
    struct queueT_NAME(queue) *q;
    size_t size = sizeof(struct queueT_NAME(queue))
        + sizeof(queueT) * (mask + 1);

    /* allocate memory */
    q = malloc(size);
    if (q == NULL) {
        return NULL;
    }

    /* initialize it */
    memset(q, 0, size);
    q->mask = mask;
    return q;
}


int
queueT_NAME(queue_dispose) (struct queueT_NAME(queue) *q) {
    if (q == NULL) {
        return -1;
    }

    free(q);
    return 0;
}
