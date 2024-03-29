#include "menu.h"
#include <chrono>
#include <sstream>
#include "rideshare.h"
#include "timeofday.h"

#define OSMCITY "Porto"

using namespace std;

void printTitle() {
  cout << ",------. ,--.,------.  ,------. ,---.  ,--.  ,--.  ,---.  ,------. "
          ",------. "
       << endl
       << "|  .--. '|  ||  .-.  | |  .---''   .-' |  '--'  | |  O  | |  .--. "
          "'|  .---' "
       << endl
       << "|  '--'.'|  ||  |  | : |  `--, `.  `-. |  .--.  ||  .-.  ||  "
          "'--'.'|  `--, "
       << endl
       << "|  | |  ||  ||  '--' | |  `---..-'    ||  |  |  ||  | |  ||  ||  | "
          "|  `---. "
       << endl
       << "`--' '--'`--'`-------' `------'`-----' `--'  `--'`--' `--'`--' "
          "'--'`------' "
       << endl;
}

void readUsers(std::vector<User *> &users, Graph<idNode> &graph) {
  ifstream clientsFile("data/clients.txt");

  string line;
  if (clientsFile.is_open()) {
    while (getline(clientsFile, line)) {
      stringstream ss(line);
      string value;
      /** id **/
      getline(ss, value, ',');
      int id = stoi(value);
      /** name **/
      string name;
      getline(ss, name, ',');
      /** route **/
      Route route;
      getline(ss, value, ',');
      route.startNode = stol(value);
      getline(ss, value, ',');
      route.endNode = stol(value);
      getline(ss, value, ',');
      int tempTime = stoi(value);
      route.arrivalTime.hour = tempTime / 60;
      route.arrivalTime.minute = tempTime - (route.arrivalTime.hour * 60);
      getline(ss, value, ',');
      tempTime = stoi(value);
      route.departureTime.hour = tempTime / 60;
      route.departureTime.minute = tempTime - (route.departureTime.hour * 60);
      getline(ss, value, ',');
      route.tolerance = stoi(value);
      /** create user **/
      User *user = new User(route, name, graph);
      getline(ss, value, ',');
      user->setSmoker(stoi(value));
      if (user->getPath().size() == 0) {
        cout << "Error: No path could be found between the two nodes" << endl;
        return;
      }
      getline(ss, value, ',');
      bool driver = stoi(value);
      getline(ss, value, ',');
      int capacity = stoi(value);
      user->setDriver(driver, capacity);
      users.push_back(user);
      user->setId(id);
    }
    clientsFile.close();
  }
}

void saveUsers(std::vector<User *> &users) {
  /** save user to file **/
  ofstream clientsFile("data/clients.txt");
  for (User *user : users) {
    clientsFile << user->getId() << "," << user->getName() << ","
                << user->getRoute().startNode << "," << user->getRoute().endNode
                << "," << user->getRoute().arrivalTime.toMinutes() << ","
                << user->getRoute().departureTime.toMinutes() << ","
                << user->getRoute().tolerance << "," << user->isSmoker() << ","
                << user->isDriver() << "," << user->getCarCapacity() << endl;
  }
  clientsFile.close();
}

void addDriver(std::vector<User *> &users) {
  unsigned int id;
  cout << "Enter your ID:";
  cin >> id;
  cout << endl;
  if (cin.fail() || id > users.size() - 1) {
    cout << "Error: Invalid id" << endl;
    return;
  }

  int carCap = 0;
  cout << "Enter car capacity:";
  cin >> carCap;
  cout << endl;
  if (cin.fail() || carCap < 0) {
    cout << "Invalid input" << endl;
    return;
  }
  users.at(id)->setDriver(true, carCap);
}

void removeDriver(std::vector<User *> &users) {
  unsigned int id;
  cout << "Enter your ID:";
  cin >> id;
  cout << endl;
  if (cin.fail() || id > users.size() - 1) {
    cout << "Error: Invalid id" << endl;
    return;
  }
  users.at(id)->setDriver(false, 0);
}

void generatePathFast(Graph<idNode> &graph, std::vector<User *> &users,
                      OSMCollection &osmCol) {
  unsigned int driverId;
  cout << "Enter driver id:";
  cin >> driverId;
  cout << endl;
  if (cin.fail() || driverId > users.size() - 1) {
    cout << "Error: Invalid id" << endl;
    return;
  }

  User *driver = users.at(driverId);
  if (!driver->isDriver()) {
    cout << "Error: User is not Driver" << endl;
    return;
  }

  std::cout << "rideshareFast" << std::endl;
  double tripTime;
  auto started = std::chrono::high_resolution_clock::now();
  std::vector<User *> passengers;
  std::list<Vertex<idNode> *> result =
      rideshareFast(graph, users, *driver, tripTime, passengers);
  auto done = std::chrono::high_resolution_clock::now();
  printPath(passengers, osmCol, result, *driver, graph);
  std::cout << "Execution time(ms): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(done -
                                                                     started)
                   .count()
            << std::endl
            << std::endl;
  if (graph.isGraphViewer()) displayPath(passengers, result, *driver);
}

void generatePathBest(Graph<idNode> &graph, std::vector<User *> &users,
                      OSMCollection &osmCol) {
  unsigned int driverId;
  cout << "Enter driver id:";
  cin >> driverId;
  cout << endl;
  if (cin.fail() || driverId > users.size() - 1) {
    cout << "Error: Invalid id" << endl;
    return;
  }

  User *driver = users.at(driverId);
  if (!driver->isDriver()) {
    cout << "Error: User is not Driver" << endl;
    return;
  }

  double tripTime;
  vector<User *> passengers;
  cout << "rideshareBest" << endl;
  auto started = chrono::high_resolution_clock::now();
  list<Vertex<idNode> *> result =
      rideshareBest(graph, users, *driver, tripTime, passengers);
  auto done = chrono::high_resolution_clock::now();
  printPath(passengers, osmCol, result, *driver, graph);
  cout << "Execution time(ms): "
       << chrono::duration_cast<chrono::milliseconds>(done - started).count()
       << endl
       << endl;
  if (graph.isGraphViewer()) displayPath(passengers, result, *driver);
}

void printGraphInfo(Graph<idNode> &graph) {
  cout << endl;
  cout << "City: " << OSMCITY << endl;
  cout << "Nodes: " << graph.vertexSetSize() << endl;
  cout << "Edges: " << graph.edgeCount() << endl;
  cout << "Inverting graph to check connectivity..." << endl;
  std::string con = graph.isStronglyConnected() ? "Yes" : "No";
  cout << "Strongly connected: " << con << endl;
  cout << endl;
}

void mainMenu(Graph<idNode> &graph, std::vector<User *> &users,
              OSMCollection &osmCol) {
  printTitle();

  int opcao = 0;
  while (opcao != 7) {
    cout << " 1 - Create new Client" << endl;
    cout << " 2 - Become driver (add car/change capacity)" << endl;
    cout << " 3 - Stop driving (remove car)" << endl;
    cout << " 4 - Generate path (rideshareFast)" << endl;
    cout << " 5 - Generate path (rideshareBest)" << endl;
    cout << " 6 - Graph information" << endl;
    cout << " 7 - exit" << endl;
    cout << endl;
    cout << "Choose an option: ";
    if (cin >> opcao) {
      switch (opcao) {
        case 1:
          createNewUser(graph, users);
          break;
        case 2:
          addDriver(users);
          break;
        case 3:
          removeDriver(users);
          break;
        case 4:
          generatePathFast(graph, users, osmCol);
          break;
        case 5:
          generatePathBest(graph, users, osmCol);
          break;
        case 6:
          printGraphInfo(graph);
          break;
        case 7:
          return;
        default:
          cout << "Invalid option. Please, choose another option: ";
      }
    } else {
      cin.clear();
      cin.ignore(10000, '\n');
      cout << "Invalid option. Please, choose another option: ";
    }
  }
}

void createNewUser(Graph<idNode> &graph, std::vector<User *> &users) {
  printTitle();
  cout << endl;
  static int userId = users.size();
  Route route;
  string name, node, nodeA, tolerance;
  bool smoke;
  string smokerInput;
  int depHours, depMinutes, tol;
  int long origin, destination;

  /** user name **/
  cout << "Insert the username of the new User: ";
  cin.ignore();
  getline(cin, name);
  /** user start node **/
  cout << "Insert the starting point: ";
  cin >> origin;
  Vertex<idNode> *tempNode;
  tempNode = graph.findVertex(origin);
  if (cin.fail() || tempNode == NULL) {
    cout << "Node does not exist in the current graph" << endl;
    return;
  }
  route.startNode = origin;
  /** user end node **/
  cout << "Insert the destination point: ";
  cin >> destination;
  tempNode = graph.findVertex(destination);
  if (cin.fail() || tempNode == NULL) {
    cout << "Node does not exist in the current graph" << endl;
    return;
  }
  route.endNode = destination;
  /** user departure time **/
  cout << "Insert the departure hour (0-23): ";
  cin >> depHours;
  if (depHours < 0 || depHours > 23 || cin.fail()) {
    cout << "Invalid hours format" << endl;
    return;
  }
  cout << "Insert the departure minute (0-59): ";
  cin >> depMinutes;
  if (depMinutes < 0 || depMinutes > 59 || cin.fail()) {
    cout << "Invalid minutes format" << endl;
    return;
  }
  route.departureTime.hour = depHours;
  route.departureTime.minute = depMinutes;
  /** user arrival time **/
  cout << "Insert the arrival hour (0-23): ";
  cin >> depHours;
  if (depHours < 0 || depHours > 23 || cin.fail()) {
    cout << "Invalid hours format" << endl;
    return;
  }
  cout << "Insert the arrival minute (0-59): ";
  cin >> depMinutes;
  if (depMinutes < 0 || depMinutes > 59 || cin.fail()) {
    cout << "Invalid minutes format" << endl;
    return;
  }
  route.arrivalTime.hour = depHours;
  route.arrivalTime.minute = depMinutes;
  /** user tolerance **/
  cout << "Insert the tolerance for arrival and departure (minutes): ";
  cin >> tol;
  if (cin.fail()) {
    cout << "Invalid minutes format" << endl;
    return;
  }
  route.tolerance = tol;
  /** user smoker status **/
  cout << "Do you smoke? (Y/N) ";
  while (smokerInput != "y" && smokerInput != "Y" && smokerInput != "n" &&
         smokerInput != "N") {
    cin.ignore();
    getline(cin, smokerInput);

    if (smokerInput == "y" || smokerInput == "Y") {
      smoke = true;
      break;
    }
    if (smokerInput == "n" || smokerInput == "N") {
      smoke = false;
      break;
    } else {
      cout << "Invalid option. Please, choose another: ";
    }
  }

  /** generate new user **/
  User *user = new User(route, name, graph);
  user->setSmoker(smoke);
  if (user->getPath().size() == 0) {
    cout << "Error: No path could be found between the two nodes" << endl;
    return;
  }
  users.push_back(user);
  user->setId(userId);
  ++userId;

  cout << endl << "User " << userId << " added with success!" << endl << endl;
}
