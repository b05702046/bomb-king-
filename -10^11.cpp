#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;
ll m, n, h;
const int MAXM = 50 + 5;
const int MAXN = 1000 + 5;
const int MAXT = 1000000 + 5;
const ll INF = MAXT + 5;
ll delay = 0;
struct machine{
    ll a;
    ll b;
    ll c;
    ll p;
    ll L;
    ll t;
};
struct order{
    ll q;
    ll d;
    ll index;
};
bool ddl(order& a, order& b){
    if (a.d < b.d)
        return true;
    else if (a.d > b.d)
        return false;
    else
        return (a.q < b.q);
}
machine mrr[MAXM];
order orr[MAXN];
int fixer[MAXT];
vector<int> answer[MAXM];
machine test(ll dmd, ll mac, bool mtc){
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
    return ans;
}
bool better(machine& x, machine& y){
    if (x.t == -1)
        return false;
    else if (x.t < y.t)
        return true;
    else if (x.t > y.t)
        return false;
    else if (x.a * x.p > y.a * y.p)
        return true;
    else if (x.a * x.p < y.a * y.p)
        return false;
    else
        return (x.b < y.b);
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
        cin >> mrr[i].a >> mrr[i].b >> mrr[i].c >> mrr[i].p >> mrr[i].L;
        mrr[i].p = max(mrr[i].L, mrr[i].p - mrr[i].b);
    }
    for (int i = 1; i <= n; i++){
        cin >> orr[i].q >> orr[i].d;
        orr[i].q *= 100;
        orr[i].index = i;
    }
    for (int i = 0; i < MAXT; i++){
        fixer[i] = h;
    }
    sort(orr + 1, orr + n + 1, ddl);
    for (int i = 1; i <= n; i++){
        machine best = {-1, -1, -1, -1, -1, INF};
        bool fix = false;
        int choice = -1;
        for (int j = 1; j <= m; j++){
            for (int k = 0; k <= 1; k++){
                machine status = test(i, j, k);
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
            cout << 0 << '\n';
        }
        else {
            for (int j = 0; j < (int)answer[i].size(); j++){
                if (answer[i][j] == -1)
                    cout << 'M';
                else
                    cout << answer[i][j];
                cout << ",\n"[j == (int)answer[i].size() - 1];
            }
        }
    }
    cerr << delay << '\n';
    return 0;
}
