#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;
ll m, n, h;
const int MAXM = 50 + 5;
const int MAXN = 1000 + 5;
const int MAXT = 1000000 + 5;
const ll INF = MAXT + 5;
const int IDs = 20;
ll delay = 0;
struct machine{
    ll a;
    ll b;
    ll c;
    ll p;
    ll L;
    ll t;
    ll leak;
};
struct order{
    ll q;
    ll d;
    ll index;
};
bool comp1(order& a, order& b){
    if (a.d < b.d)
        return true;
    else if (a.d > b.d)
        return false;
    else
        return (a.q < b.q);
}
bool comp2(order& a, order& b){
    if (a.d < b.d)
        return true;
    else if (a.d > b.d)
        return false;
    else
        return (a.q > b.q);
}
bool comp3(order& a, order& b){
    if (a.q < b.q)
        return true;
    else if (a.q > b.q)
        return false;
    else
        return (a.d < b.d);
}
bool comp4(order& a, order& b){
    if(a.q > 20000)
        return false;
    else if (a.d < b.d)
        return true;
    else if (a.d > b.d)
        return false;
    else
        return (a.q < b.q);
}
bool comp5(order& a, order& b){
    if(a.q > 40000)
        return false;
    else if (a.d < b.d)
        return true;
    else if (a.d > b.d)
        return false;
    else
        return (a.q < b.q);
}
machine mrr[MAXM];
machine initialMRR[MAXM];
vector<int> answer[MAXM];
order orr[MAXN];
int fixer[MAXT];
machine test(ll dmd, ll mac, bool mtc, ll deadline){
    machine ans = mrr[mac];
    if (mtc){
        bool flag = true;
        for (int i = ans.t + 1; i <= ans.t + ans.c; i++){
            flag &= (fixer[i] > 0);
        }
        if (flag){
            ans.t += ans.c;
            ans.p = 100;
        }
        else {
            ans.t = -1;
            return ans;
        }
    }
    ll cnt = 0;
    while (cnt < orr[dmd].q){
        cnt += ans.a * ans.p;
        ans.p = max(ans.L, ans.p - ans.b);
        ans.t++;
    }
    ans.leak = cnt - orr[dmd].q;
    if(ans.t <= deadline)
        ans.t = 0;
    return ans;
}
bool better(machine& x, machine& y){
    if (x.t == -1)
        return false;
    else if (x.t < y.t)
        return true;
    else if (x.t > y.t)
        return false;
    else if (x.leak <= y.leak)
        return true;
    else
        return false;
}
void update(ll dmd, ll mac, bool mtc){
//  cerr << "\nOrder " << orr[dmd].index << " assigned to machine " << mac << ":\n";
    if (mtc){
        bool flag = true;
        for (int i = mrr[mac].t + 1; i <= mrr[mac].t + mrr[mac].c; i++){
            fixer[i]--;
//          cerr << "Maintenance...\n";
        }
        if (flag){
            mrr[mac].t += mrr[mac].c;
            mrr[mac].p = 100;
        }
        else {
            mrr[mac].t = -1;
            return;
        }
    }
    ll cnt = 0;
    while (cnt < orr[dmd].q){
        cnt += mrr[mac].a * mrr[mac].p;
        mrr[mac].p = max(mrr[mac].L, mrr[mac].p - mrr[mac].b);
        mrr[mac].t++;
//      cerr << "cnt += " << mrr[mac].a * mrr[mac].p << '\n';
    }
//  fprintf(stderr, "Delayed %lld: Finish %lld / Deadline %lld\n", max(mrr[mac].t - orr[dmd].d, 0LL), mrr[mac].t, orr[dmd].d);
    delay += max(mrr[mac].t - orr[dmd].d, 0LL);
    return;
}
int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    cin >> m >> n >> h;
    for (int i = 1; i <= m; i++){
        cin >> initialMRR[i].a >> initialMRR[i].b >> initialMRR[i].c >> initialMRR[i].p >> initialMRR[i].L;
        initialMRR[i].p = max(initialMRR[i].L, initialMRR[i].p - initialMRR[i].b);
    }
    for (int i = 1; i <= n; i++){
        cin >> orr[i].q >> orr[i].d;
        orr[i].q *= 100;
        orr[i].index = i;
    }
         
    int*** finalAns = new int**[IDs];
    for(int i = 0; i < IDs; i++)
    {
        finalAns[i] = new int*[m + 1];
    }
    int* delays = new int[IDs];
    for(int testID = 0; testID < IDs; testID++)
    {
        delay = 0;
        for (int i = 1; i <= m; i++){
            mrr[i].a = initialMRR[i].a;
            mrr[i].b = initialMRR[i].b;
            mrr[i].c = initialMRR[i].c;
            mrr[i].p = initialMRR[i].p;
            mrr[i].L = initialMRR[i].L;
            mrr[i].t = 0;
            mrr[i].leak = 0;
        }
        for (int i = 0; i < MAXT; i++){
            fixer[i] = h;
        }
             
        if(testID == 0)
            sort(orr + 1, orr + n + 1, comp1);
        else if(testID == 1)
            sort(orr + 1, orr + n + 1, comp2);
        else if(testID == 2)
            sort(orr + 1, orr + n + 1, comp3);
        else if(testID == 3)
            sort(orr + 1, orr + n + 1, comp4);
        else if(testID == 4)
            sort(orr + 1, orr + n + 1, comp5);
        else if(testID == 5){
            sort(orr + 1, orr + n + 1, comp1);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 6){
            sort(orr + 1, orr + n + 1, comp2);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 7){
            sort(orr + 1, orr + n + 1, comp3);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 8){
            sort(orr + 1, orr + n + 1, comp4);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 9){
            sort(orr + 1, orr + n + 1, comp5);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 10){
            sort(orr + 1, orr + n + 1, comp1);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 11){
            sort(orr + 1, orr + n + 1, comp2);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 12){
            sort(orr + 1, orr + n + 1, comp3);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 13){
            sort(orr + 1, orr + n + 1, comp4);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 14){
            sort(orr + 1, orr + n + 1, comp5);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 15){
            sort(orr + 1, orr + n + 1, comp1);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 16){
            sort(orr + 1, orr + n + 1, comp2);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 17){
            sort(orr + 1, orr + n + 1, comp3);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 18){
            sort(orr + 1, orr + n + 1, comp4);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
        else if(testID == 19){
            sort(orr + 1, orr + n + 1, comp5);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
            for(int i = 1; i < n; i++)
                swap(orr[i], orr[i + 1]);
        }
         
        for (int i = 1; i <= n; i++){
            machine best = {-1, -1, -1, -1, -1, INF};
            bool fix = false;
            int choice = -1;
            for (int j = 1; j <= m; j++){
                for (int k = 0; k <= 1; k++){
                    machine status = test(i, j, k, orr[i].d);
                    if (better(status, best)){
                        best = status;
                        fix = k;
                        choice = j;
                    }
                }
            }
            if (fix)
                answer[choice].push_back(-1);
            answer[choice].push_back(orr[i].index);
            update(i, choice, fix);
        }
        for (int i = 1; i <= m; i++){
                 
            if (answer[i].empty()){
                finalAns[testID][i] = new int[2];
                finalAns[testID][i][0] = 1;
                finalAns[testID][i][1] = 0;
            }
            else {
                int assignCnt = 0;
                finalAns[testID][i] = new int[(int)answer[i].size() + 1];
                for (int j = 0; j < (int)answer[i].size(); j++){
                    finalAns[testID][i][j + 1] = answer[i][j];
                    assignCnt += 1;
                }
                finalAns[testID][i][0] = assignCnt;
            }
            answer[i].erase(answer[i].begin(), answer[i].begin() + (int)answer[i].size());
        }
        delays[testID] = delay;
    }
    int minDelay = delays[0];
    int minDelayOrd = 0;
    for(int i = 1; i < IDs; i++)
        if(delays[i] < minDelay){
            minDelay = delays[i];
            minDelayOrd = i;
        }
    for (int i = 1; i <= m; i++){
        for (int j = 1; j <= finalAns[minDelayOrd][i][0]; j++){
            if (finalAns[minDelayOrd][i][j] == -1)
                cout << 'M';
            else
                cout << finalAns[minDelayOrd][i][j];
            cout << ",\n"[j == finalAns[minDelayOrd][i][0]];
        }
    }
    cerr << minDelay << '\n';
    return 0;
}
