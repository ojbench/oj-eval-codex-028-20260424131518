#include <bits/stdc++.h>
using namespace std;

struct Student {
    string name;
    char gender; // 'M' or 'F'
    int cls;
    int score[9]{};
    int avg{0}; // floor average
};

// For ranking, comparator based on rules
struct RankCmp {
    const unordered_map<string, Student>* pmap;
    RankCmp(const unordered_map<string, Student>* m=nullptr): pmap(m) {}
    bool operator()(const string &a, const string &b) const {
        if (a == b) return false;
        const Student &sa = pmap->at(a);
        const Student &sb = pmap->at(b);
        if (sa.avg != sb.avg) return sa.avg > sb.avg; // higher first
        for (int i=0;i<9;i++) {
            if (sa.score[i] != sb.score[i]) return sa.score[i] > sb.score[i];
        }
        return sa.name < sb.name; // lexicographically smaller first
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    unordered_map<string, Student> mp;
    vector<string> ranking; // list of names sorted
    unordered_map<string,int> pos; // name -> 1-based rank
    bool started = false;
    bool dirty = false; // ranking needs refresh

    auto recompute_avg = [](Student &s){
        long long sum=0; for(int i=0;i<9;i++) sum += s.score[i];
        s.avg = int(sum/9);
    };

    auto rebuild_ranking = [&](){
        ranking.clear(); ranking.reserve(mp.size());
        for (auto &kv : mp) ranking.push_back(kv.first);
        RankCmp cmp(&mp);
        sort(ranking.begin(), ranking.end(), cmp);
        pos.clear(); pos.reserve(ranking.size()*2+1);
        for (size_t i=0;i<ranking.size();++i) pos[ranking[i]] = int(i)+1;
        dirty = false;
    };

    string cmd;
    while (cin >> cmd) {
        if (cmd == "ADD") {
            string name; char genderChar; int cls; int sc[9];
            cin >> name >> genderChar >> cls;
            for (int i=0;i<9;i++) cin >> sc[i];
            if (started) {
                cout << "[Error]Cannot add student now." << '\n';
                continue;
            }
            if (mp.find(name) != mp.end()) {
                cout << "[Error]Add failed." << '\n';
                continue;
            }
            Student s; s.name=name; s.gender=genderChar; s.cls=cls;
            for(int i=0;i<9;i++) s.score[i]=sc[i];
            recompute_avg(s);
            mp.emplace(name, std::move(s));
        } else if (cmd == "START") {
            started = true;
            dirty = true;
            rebuild_ranking();
        } else if (cmd == "UPDATE") {
            string name; int code, score;
            cin >> name >> code >> score;
            auto it = mp.find(name);
            if (it == mp.end()) {
                cout << "[Error]Update failed." << '\n';
                continue;
            }
            it->second.score[code] = score;
            recompute_avg(it->second);
            // Do not auto-refresh ranking until FLUSH
            dirty = true;
        } else if (cmd == "FLUSH") {
            if (dirty) rebuild_ranking();
        } else if (cmd == "PRINTLIST") {
            // Do not change ranking order if dirty; only FLUSH updates order
            for (size_t i=0;i<ranking.size();++i) {
                const Student &s = mp.at(ranking[i]);
                cout << (i+1) << ' ' << s.name << ' ' << (s.gender=='M'?"male":"female")
                     << ' ' << s.cls << ' ' << s.avg << '\n';
            }
        } else if (cmd == "QUERY") {
            string name; cin >> name;
            auto it = mp.find(name);
            if (it == mp.end()) {
                cout << "[Error]Query failed." << '\n';
                continue;
            }
            int r = pos[name];
            cout << "STUDENT " << name << " NOW AT RANKING " << r << '\n';
        } else if (cmd == "END") {
            break;
        } else {
            // Unknown command, ignore gracefully
        }
    }

    return 0;
}
