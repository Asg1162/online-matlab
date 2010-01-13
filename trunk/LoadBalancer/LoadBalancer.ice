
module Matcloud {

       sequence<string> StrArray;

       enum QueryCmd { QueryUser, QueryCluster}; 

       interface Backend {
                 void addNode(string newnode);
                 void delNode(string obsnode);
       };

       interface Query {
/*            StrArray queryUser();
            StrArray queryCluster();*/
            int queryNbNodes();
       };
       interface Frontend {
                 string FrontendAgent(string cmd);
       };

};
