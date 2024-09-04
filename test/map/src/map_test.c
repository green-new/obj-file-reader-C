#include "material_map.h"

int test_map_insert(mat_map* map) {
    int code = SUCCESS;
    // Dummy material object
    mtl_t dummy = mtl_create();
    // Different key names
    if ((code = map_insert(map, "Wood", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Dirt", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Stone", dummy)) != SUCCESS) {
        return code;
    }
    return code;
}

int test_map_at(mat_map* map) {
    mtl_t* dummy = NULL;
    map_at(map, "Wood", &dummy);
    if (dummy == NULL) {
        return 0;
    }
    map_at(map, "Dirt", &dummy);
    if (dummy == NULL) {
        return 0;
    }
    map_at(map, "Stone", &dummy);
    if (dummy == NULL) {
        return 0;
    }
    return SUCCESS;
}

int test_map_empty(mat_map* map) {
    return map_empty(map) == 0;
}

int test_map_erase(mat_map* map) {
    int code;
    map_erase(map, "Stone");
    mtl_t* temp = NULL;
    if ((code = map_at(map, "Stone", &temp)) != NOT_FOUND) {
        return 0;
    }
    return SUCCESS;
}

int test_map_size(mat_map* map) {
    return map_size(map) == 2;
}

int test_map_clear(mat_map* map) {
    map_clear(map);
    return map_size(map) == 0;
}

int test_map_bulkinsert(mat_map* map) {
    int code = SUCCESS;
    // Dummy material object
    mtl_t dummy = mtl_create();
    // Different key names
    if ((code = map_insert(map, "Wood", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Dirt", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Stone", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Gravel", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Lava", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Plastic", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Silver", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Gold", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Tin", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Platinum", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Paper", dummy)) != SUCCESS) {
        return code;
    }
    if ((code = map_insert(map, "Aluminum", dummy)) != SUCCESS) {
        return code;
    }
    return code;
}

int test_map_copy(mat_map* map) {
    mat_map map2;
    map_copy(&map2, map);
    return map_size(&map2) == map_size(map);
}

int test_keyslist(mat_map* map) {
    keys_list_t list = map_keys(map);
    if (list.used != map_size(map)) {
        keys_list_destroy(&list);
        return 0;
    }
    printf("[");
    const char* sep = "";
    for (unsigned int i = 0; i < list.used; i++) {
        printf("%s", sep);
        printf("%s", list.keys[i]);
        sep = ", ";
    }
    printf("]\n");
    keys_list_destroy(&list);
    return SUCCESS;
}

int main() {
    int code = SUCCESS;
    mat_map map;
    if ((code = map_create(&map) != SUCCESS)) {
        printf("Map creation failed\n");
        return code;
    }
    if ((code = test_map_insert(&map) != SUCCESS)) {
        printf("Map insertion failed\n");
        return code;
    }
    if ((code = test_map_at(&map) != SUCCESS)) {
        printf("Map at() failed\n");
        return code;
    }
    if ((code = test_map_empty(&map) != SUCCESS)) {
        printf("Map empty failed\n");
        return code;
    }
    if ((code = test_map_erase(&map) != SUCCESS)) {
        printf("Map erase failed\n");
        return code;
    }
    if ((code = test_map_size(&map) != SUCCESS)) {
        printf("Map size failed\n");
        return code;
    }
    if ((code = test_map_clear(&map) != SUCCESS)) {
        printf("Map clear failed\n");
        return code;
    }
    if ((code = test_map_bulkinsert(&map) != SUCCESS)) {
        printf("Map bulk insert failed\n");
        return code;
    }
    if ((code = test_map_copy(&map) != SUCCESS)) {
        printf("Map copy failed\n");
        return code;
    }
    if ((code = test_keyslist(&map) != SUCCESS)) {
        printf("Map keys list failed\n");
        return code;
    }
    
    return 0;
}