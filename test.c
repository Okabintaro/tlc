void entities_save(const char *path) {
       FILE *f = fopen(path, "wb");
       for (int i = 0; i < entities_len; i++) {
               entity_t ent = entities_storage[i];
               int written = fwrite(&ent, sizeof(ent), 1, f);
               if (written != 1) {
                       const char *msg = strerror(errno);
                       fprintf(stderr, "Error writing entity to file: %s\n", msg);
               }
       }
       fclose(f);
}


void entities_load(const char *path) {
       int i = 0;
       FILE *f = fopen(path, "rb");
       entities_reset();

       for (i = 0; i < ENTITIES_MAX; i++) {
               entity_t ent = {0};
               int read = fread(&ent, sizeof(ent), 1, f);
               if (read != 1) {
                       if (ferror(f)) {
                               const char *msg = strerror(errno);
                               fprintf(stderr, "Error reading entity from file: %s\n", msg);
                       } else {
                               printf("Reached end of file\n");
                               break;
                       }
               }

               entity_t tmp_ent;
               entity_vtab[ent.type].init(&tmp_ent);
               ent.anim.def = tmp_ent.anim.def;
               entities_storage[i] = ent;
       }
       fclose(f);
       entities_len = i;
       printf("Loaded %d entities\n", i);
}
