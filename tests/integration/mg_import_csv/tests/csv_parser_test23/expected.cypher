CREATE INDEX ON :__mg_vertex__(__mg_id__);
CREATE (:__mg_vertex__ {__mg_id__: 0, `test`: "asd", `id`: "0", `value`: "wo\"rld"});
CREATE (:__mg_vertex__ {__mg_id__: 1, `test`: "string", `id`: "1"});
CREATE (:__mg_vertex__ {__mg_id__: 2, `test`: "wil\"l this work?\"\"", `id`: "2", `value`: "hello"});
DROP INDEX ON :__mg_vertex__(__mg_id__);
MATCH (u) REMOVE u:__mg_vertex__, u.__mg_id__;
