CREATE INDEX ON :__mg_vertex__(__mg_id__);
CREATE (:__mg_vertex__ {__mg_id__: 0});
CREATE (:__mg_vertex__ {__mg_id__: 1});
CREATE (:__mg_vertex__ {__mg_id__: 2});
CREATE (:__mg_vertex__ {__mg_id__: 3});
CREATE (:__mg_vertex__:`label` {__mg_id__: 4});
CREATE (:__mg_vertex__:`label` {__mg_id__: 5});
CREATE (:__mg_vertex__:`lab` {__mg_id__: 6});
CREATE (:__mg_vertex__:`lab` {__mg_id__: 7});
CREATE (:__mg_vertex__:`lab` {__mg_id__: 8});
CREATE (:__mg_vertex__:`lab2` {__mg_id__: 9});
CREATE (:__mg_vertex__:`lab2` {__mg_id__: 10});
CREATE (:__mg_vertex__:`lab2` {__mg_id__: 11});
CREATE (:__mg_vertex__ {__mg_id__: 12});
CREATE (:__mg_vertex__ {__mg_id__: 13});
CREATE (:__mg_vertex__ {__mg_id__: 14});
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 0 AND v.__mg_id__ = 1 CREATE (u)-[:`link`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 2 AND v.__mg_id__ = 3 CREATE (u)-[:`link2`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 4 AND v.__mg_id__ = 5 CREATE (u)-[:`link`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 4 AND v.__mg_id__ = 4 CREATE (u)-[:`link`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 4 AND v.__mg_id__ = 5 CREATE (u)-[:`link`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 4 AND v.__mg_id__ = 4 CREATE (u)-[:`link2`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 4 AND v.__mg_id__ = 5 CREATE (u)-[:`link2`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 5 AND v.__mg_id__ = 4 CREATE (u)-[:`link`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 5 AND v.__mg_id__ = 5 CREATE (u)-[:`link`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 5 AND v.__mg_id__ = 4 CREATE (u)-[:`link2`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 5 AND v.__mg_id__ = 5 CREATE (u)-[:`link2`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 8 AND v.__mg_id__ = 13 CREATE (u)-[:`link88`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 8 AND v.__mg_id__ = 11 CREATE (u)-[:`link88`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 8 AND v.__mg_id__ = 12 CREATE (u)-[:`link88`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 9 AND v.__mg_id__ = 11 CREATE (u)-[:`link88`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 9 AND v.__mg_id__ = 12 CREATE (u)-[:`link88`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 9 AND v.__mg_id__ = 13 CREATE (u)-[:`link88`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 10 AND v.__mg_id__ = 9 CREATE (u)-[:`link3`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 10 AND v.__mg_id__ = 11 CREATE (u)-[:`link88`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 10 AND v.__mg_id__ = 12 CREATE (u)-[:`link88`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 10 AND v.__mg_id__ = 13 CREATE (u)-[:`link88`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 11 AND v.__mg_id__ = 12 CREATE (u)-[:`link3`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 11 AND v.__mg_id__ = 14 CREATE (u)-[:`selfedge`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 11 AND v.__mg_id__ = 11 CREATE (u)-[:`selfedge2`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 12 AND v.__mg_id__ = 13 CREATE (u)-[:`link3`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 12 AND v.__mg_id__ = 12 CREATE (u)-[:`selfedge2`]->(v);
MATCH (u:__mg_vertex__), (v:__mg_vertex__) WHERE u.__mg_id__ = 13 AND v.__mg_id__ = 13 CREATE (u)-[:`selfedge2`]->(v);
DROP INDEX ON :__mg_vertex__(__mg_id__);
MATCH (u) REMOVE u:__mg_vertex__, u.__mg_id__;
