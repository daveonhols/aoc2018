#include <algorithm>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <vector>

// solver policy object for part 1
struct w_policy_p1 {
  int num = 1;
  int dur(char t) { return 0; };
};

// solver policy object for part 2
struct w_policy_p2 {
  int num = 5;
  int dur(char t) { return 60 + 1 + t - 'A'; };
};

// hold the result order and run time
struct solution_t {
  int duration;
  std::vector<char> order;
};

// keep track of dependencies in both directions and building solution from
// that.
class depends {

private:
  // Step C must be finished before step A can begin
  std::multimap<char, char> dependsOn{};  // <A,C>
  std::multimap<char, char> dependedOn{}; // <C,A>

  std::set<char> unique{};
  std::set<char> free{};

public:
  void init();
  std::vector<char> get_resolved(char c);
  bool last_required(char r, char d);
  void remove_one(char p, char a);
  void parse_add(std::string dependency);
  void add(char d_on, char d_by);
  void remove(char c);
  int uc() { return unique.size(); };
  std::optional<char> next();

  template <class P> solution_t solve(P policy);
  //  std::vector<char> solve_p2(int w, int dur);
};

std::vector<char> depends::get_resolved(char c) {
  std::vector<char> res{};
  auto range = dependedOn.equal_range(c);
  for (auto i_affected = range.first; i_affected != range.second;
       ++i_affected) {
    char found = i_affected->second;
    res.push_back(found);
  }
  return res;
}

// depended depends on removed, but is removed the last dependency, if so we can
// mark depended as free
bool depends::last_required(char removed, char depended) {
  auto requirements = dependsOn.equal_range(depended);
  if (requirements.first != --requirements.second) {
    return false; // not the last dep
  }
  return (requirements.first->first == depended) &&
         (requirements.first->second == removed);
}

// we only need to remove from dependsOn for the algo to work...
void depends::remove_one(char prior, char after) {
  auto range = dependsOn.equal_range(after);
  for (auto it = range.first; it != range.second;) {
    if (it->second == prior) {
      dependsOn.erase(it++);
    } else {
      ++it;
    }
  }
  return;
}

void depends::remove(char c) {
  // we remove C, look in dependedOn for all X that depend on C
  // then go to dependsOn and remove <X, C>.
  // our next loop round needs to consider X as affected and check if it's empty
  // in dependsOn
  std::vector<char> resolved = get_resolved(c);
  std::vector<char> fully_resolved;

  // check those tasks which have had an upstream dependency removed
  // to see if the removed dependency was the final one
  // if it was, they can become free
  std::copy_if(resolved.begin(), resolved.end(),
               std::back_inserter(fully_resolved),
               [this, c](char depended) { return last_required(c, depended); });

  // for each resolved, remove that dependency from graph
  std::for_each(resolved.begin(), resolved.end(),
                [this, c](char resolved) { remove_one(c, resolved); });

  // all those fully resolved should go to free list
  std::copy(fully_resolved.begin(), fully_resolved.end(),
            std::inserter(free, free.begin()));

  return;
}

template <class P> solution_t depends::solve(P policy) {
  int workers = policy.num;
  std::vector<char> res;
  init();
  // finish at, task
  std::multimap<int, char> in_progress{};
  int time = 0;
  std::optional<char> n = next();
  while (dependsOn.size() > 0) {
    while (n) {
      in_progress.insert({time + policy.dur(*n), *n});
      --workers;
      if (workers == 0) {
        break;
      }
      n = next();
    }

    auto first_done = in_progress.begin();
    time = first_done->first;
    auto all_done = in_progress.equal_range(time);
    for (auto it = all_done.first; it != all_done.second;) {
      workers++;
      res.push_back(it->second);
      remove(it->second);
      in_progress.erase(it++);
    }
    n = next();
  }

  res.push_back(*n);

  solution_t sol{};
  sol.duration = time + policy.dur(*n);
  sol.order = res;
  //  std::cout << "last = " << *n << "dur = " << time+policy.dur(*n) <<
  //  std::endl;
  return sol;
}

int duration(char task) { return 1 + task - 'A'; }

void depends::init() {
  for (auto c : unique) {
    if (0 == dependsOn.count(c)) {
      free.insert(c);
    }
  }
}

std::optional<char> depends::next() {
  auto nfree = free.begin();
  auto res = std::optional<char>{};
  if (free.size() > 0) {
    res = std::optional<char>{*nfree};
    free.erase(nfree);
  }
  return res;
}

void depends::add(char d_on, char d_by) {
  dependsOn.insert({d_by, d_on});
  dependedOn.insert({d_on, d_by});
  unique.insert(d_on);
  unique.insert(d_by);
  return;
}

void depends::parse_add(std::string dependency) {
  char d_on = dependency[5];
  char d_by = dependency[36];
  add(d_on, d_by);
  return;
}

// Step I must be finished before step G can begin
