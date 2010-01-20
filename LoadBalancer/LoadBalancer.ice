
module Matcloud {

       sequence<string> StrArray;

       enum QueryCmd { QueryUser, QueryCluster}; 

       /* load balancer (server), query tools (client) */
       interface Query {
/*            StrArray queryUser();
            StrArray queryCluster();*/
            int queryNbNodes();
       };

       /* load balancer(server), web server(client) */
       interface Frontend {
                 string FrontendAgent(string cmd);
				 string FrontendLogin(string cmd);
				 string FrontendLogout(string cmd);
				 string FrontendCalc(string cmd);
       };


       /* load balancer(server), backend(client) */
       interface Backend {
                 void addNode(string newnode);
                 void delNode(string obsnode);
       };

       /* backend (server), load balancer(client) */
       interface Parser {
                 string command(string cmd);
       };

};
