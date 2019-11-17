
#include <fstream>
#include <iostream>
#include <stack>
#include <vector>

class Automata {
 private:
  struct Edge {
    Edge(int from, int to, char symbol);

    int from;
    int to;
    char symbol;
  };

  struct Node {
    Node(bool start = false, bool terminal = false);

    bool start;
    bool terminal;
    std::vector<Edge> edges;
  };

 public:
  Automata(const std::string& regex);

  bool FindPrefix(char letter, int number);

 private:
  int AddNode(bool start = false, bool terminal = false);
  void AddEdge(int from, int to, char symbol);
  void Concat(std::stack<int>& start_nodes, std::stack<int>& terminal_nodes);
  void Plus(std::stack<int>& start_nodes, std::stack<int>& terminal_nodes);
  void Kleene(std::stack<int>& start_nodes, std::stack<int>& terminal_nodes);
  bool Visit(int node, char letter, int number, std::vector<bool> visited,
             std::vector<int> last_state);
 private:
  std::vector<Node> nodes;
  int start;
  int terminal;
};

Automata::Edge::Edge(int from, int to, char symbol)
    : from{from}, to{to}, symbol{symbol} {}

Automata::Node::Node(bool start, bool terminal)
    : start{start}, terminal{terminal} {}

Automata::Automata(const std::string& regex) {
  std::stack<int> start_nodes;
  std::stack<int> terminal_nodes;
  for (char symbol : regex) {
    if (('a' <= symbol && symbol <= 'c') || symbol == '1') {
      start_nodes.push(AddNode(true, false));
      terminal_nodes.push(AddNode(false, true));
      AddEdge(start_nodes.top(), terminal_nodes.top(), symbol);
      continue;
    }
    if (symbol == '.') {
      Concat(start_nodes, terminal_nodes);
      continue;
    }
    if (symbol == '+') {
      Plus(start_nodes, terminal_nodes);
      continue;
    }
    if (symbol == '*') {
      Kleene(start_nodes, terminal_nodes);
      continue;
    }
  }
  start = start_nodes.top();
  terminal = terminal_nodes.top();
}

int Automata::AddNode(bool start, bool terminal) {
  nodes.emplace_back(start, terminal);
  return nodes.size() - 1;
}

void Automata::AddEdge(int from, int to, char symbol) {
  nodes[from].edges.emplace_back(from, to, symbol);
}

void Automata::Concat(std::stack<int>& start_nodes,
                      std::stack<int>& terminal_nodes) {
  int second_start = start_nodes.top();
  int second_terminal = terminal_nodes.top();
  start_nodes.pop();
  terminal_nodes.pop();
  int first_terminal = terminal_nodes.top();
  terminal_nodes.pop();

  nodes[second_start].start = false;
  nodes[first_terminal].terminal = false;
  AddEdge(first_terminal, second_start, '1');

  terminal_nodes.push(second_terminal);
}

void Automata::Plus(std::stack<int>& start_nodes,
                    std::stack<int>& terminal_nodes) {
  int second_start = start_nodes.top();
  int second_terminal = terminal_nodes.top();
  start_nodes.pop();
  terminal_nodes.pop();
  int first_start = start_nodes.top();
  int first_terminal = terminal_nodes.top();
  start_nodes.pop();
  terminal_nodes.pop();

  nodes[first_start].start = nodes[first_terminal].terminal =
  nodes[second_start].start = nodes[second_terminal].terminal = false;

  start_nodes.push(AddNode(true, false));
  terminal_nodes.push(AddNode(false, true));

  AddEdge(start_nodes.top(), first_start, '1');
  AddEdge(start_nodes.top(), second_start, '1');
  AddEdge(first_terminal, terminal_nodes.top(), '1');
  AddEdge(second_terminal, terminal_nodes.top(), '1');
}

void Automata::Kleene(std::stack<int>& start_nodes,
                      std::stack<int>& terminal_nodes) {
  AddEdge(terminal_nodes.top(), start_nodes.top(), '1');
  nodes[terminal_nodes.top()].terminal = false;
  terminal_nodes.pop();
  nodes[start_nodes.top()].terminal = true;
  terminal_nodes.push(start_nodes.top());
}

bool Automata::FindPrefix(char letter, int number) {
  std::vector<bool> visited(nodes.size(), false);
  std::vector<int> last_state(nodes.size(), number);
  return Visit(start, letter, number, visited, last_state);
}

bool Automata::Visit(int node, char letter, int number,
                     std::vector<bool> visited, std::vector<int> last_state) {
  if (number == 0) {
    return true;
  }
  visited[node] = true;
  last_state[node] = number;
  for (auto edge : nodes[node].edges) {
    if (visited[edge.to]) {
      return last_state[edge.to] < number;
    }
    if (edge.symbol == letter &&
        Visit(edge.to, letter, number - 1, visited, last_state)) {
      return true;
    }
    if (edge.symbol == '1' &&
        Visit(edge.to, letter, number, visited, last_state)) {
      return true;
    }
  }
  return false;
}


int main() {
  const std::string input_file = "input";
  std::fstream in(input_file);

  std::string regex;
  in >> regex;
  char letter = '\0';
  in >> letter;
  int number = 0;
  in >> number;

  std::cout << regex << std::endl;
  std::cout << letter << std::endl;
  std::cout << number << std::endl;

  Automata automata(regex);

  std::cout << (automata.FindPrefix(letter, number) ? "YES" : "NO")
            << std::endl;

  return 0;
}
