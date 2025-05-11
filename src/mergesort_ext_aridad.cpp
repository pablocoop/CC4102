#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <cstdint>
#include "contadores.h"

using namespace std;
const size_t INT64_SIZE = sizeof(int64_t);

// Ordenamiento en memoria interna (merge sort con programación dinámica)
void merge_sort_internal(vector<int64_t>& A, int /*dummy_l*/, int /*dummy_r*/) {
    size_t n = A.size();
    if (n < 2) return;
    vector<int64_t> tmp(n);

    // width = tamaño de los bloques a mergear: 1, 2, 4, 8, …
    for (size_t width = 1; width < n; width <<= 1) {
        // recorremos todo el array en saltos de 2*width
        for (size_t left = 0; left < n; left += 2 * width) {
            size_t mid   = min(left + width, n);
            size_t right = min(left + 2 * width, n);

            size_t i = left, j = mid, k = left;
            // merge de A[left..mid) y A[mid..right) en tmp[left..right)
            while (i < mid && j < right) {
                if (A[i] <= A[j]) tmp[k++] = A[i++];
                else              tmp[k++] = A[j++];
            }
            while (i < mid)  tmp[k++] = A[i++];
            while (j < right) tmp[k++] = A[j++];
        }
        // intercambiamos A y tmp para la siguiente iteración
        A.swap(tmp);
    }
}

void read_block(ifstream &f, vector<int64_t> &buf, size_t cnt) {
    f.read(reinterpret_cast<char*>(buf.data()), cnt * INT64_SIZE);
    if (f.gcount() > 0) ++read_count;
}

void write_block(ofstream &f, const vector<int64_t> &buf, size_t cnt) {
    f.write(reinterpret_cast<const char*>(buf.data()), cnt * INT64_SIZE);
    ++write_count;
}

vector<string> create_initial_runs(const string &in_fname, size_t M_bytes) {
    ifstream fin(in_fname, ios::binary);
    fin.seekg(0, ios::end);
    size_t total_bytes = fin.tellg();
    fin.seekg(0, ios::beg);

    size_t total_elems = total_bytes / INT64_SIZE;
    size_t chunk_elems = M_bytes / INT64_SIZE;
    const size_t B = 4096;

    vector<string> runs;
    size_t remaining = total_elems;
    size_t run_id = 0;

    while (remaining > 0) {
        size_t cnt = min(chunk_elems, remaining);
        vector<int64_t> buffer(cnt);

        // ----- LECTURA POR BLOQUES DE 4096 B -----
        char* buf_ptr = reinterpret_cast<char*>(buffer.data());
        size_t bytes_to_read = cnt * INT64_SIZE;
        size_t offset = 0;
        while (offset < bytes_to_read) {
            size_t to_read = min(bytes_to_read - offset, B);
            fin.read(buf_ptr + offset, to_read);
            ++read_count;                  // un I/O por cada bloque de 4 096 B
            offset += to_read;
        }

        // ordenamiento en memoria principal
        if (cnt > 1) merge_sort_internal(buffer, 0, cnt - 1);

        // ----- ESCRITURA POR BLOQUES DE 4096 B -----
        string run_name = "run_" + to_string(run_id++) + ".bin";
        ofstream fout(run_name, ios::binary);

        offset = 0;
        while (offset < bytes_to_read) {
            size_t to_write = min(bytes_to_read - offset, B);
            fout.write(buf_ptr + offset, to_write);
            ++write_count;                 // un I/O por cada bloque de 4 096 B
            offset += to_write;
        }
        fout.close();

        runs.push_back(run_name);
        remaining -= cnt;
    }

    fin.close();
    return runs;
}

struct HeapNode { int64_t value; size_t idx; bool operator>(HeapNode const &o) const { return value > o.value; } };

vector<string> k_way_merge(const vector<string> &runs, size_t d, size_t M_bytes) {
    vector<string> cur = runs;
    size_t pass = 0;
    while (cur.size() > 1) {
        vector<string> next;
        for (size_t i = 0; i < cur.size(); i += d) {
            vector<string> group;
            for (size_t j = i; j < i + d && j < cur.size(); ++j)
                group.push_back(cur[j]);
            size_t k = group.size();
            size_t buf_elems = max<size_t>(1, (M_bytes / INT64_SIZE) / (k + 1));
            vector<vector<int64_t>> in_buf(k, vector<int64_t>(buf_elems));
            vector<size_t> in_cnt(k), in_pos(k);
            vector<int64_t> out_buf;
            out_buf.reserve(buf_elems);
            vector<ifstream> fins(k);
            for (size_t r = 0; r < k; ++r) {
                fins[r].open(group[r], ios::binary);
                fins[r].read(reinterpret_cast<char*>(in_buf[r].data()), buf_elems * INT64_SIZE);
                in_cnt[r] = fins[r].gcount() / INT64_SIZE;
                if (in_cnt[r] > 0) ++read_count;
                in_pos[r] = 0;
            }
            priority_queue<HeapNode, vector<HeapNode>, greater<HeapNode>> pq;
            for (size_t r = 0; r < k; ++r) if (in_pos[r] < in_cnt[r]) pq.push({in_buf[r][in_pos[r]++], r});
            string out_name = "merged_" + to_string(pass) + "_" + to_string(i/d) + ".bin";
            ofstream fout(out_name, ios::binary);
            while (!pq.empty()) {
                auto top = pq.top(); pq.pop();
                out_buf.push_back(top.value);
                if (out_buf.size() == buf_elems) {
                    write_block(fout, out_buf, buf_elems);
                    out_buf.clear();
                }
                size_t ridx = top.idx;
                if (in_pos[ridx] < in_cnt[ridx]) pq.push({in_buf[ridx][in_pos[ridx]++], ridx});
                else if (fins[ridx].good()) {
                    fins[ridx].read(reinterpret_cast<char*>(in_buf[ridx].data()), buf_elems * INT64_SIZE);
                    in_cnt[ridx] = fins[ridx].gcount() / INT64_SIZE;
                    if (in_cnt[ridx] > 0) ++read_count;
                    in_pos[ridx] = 0;
                    if (in_cnt[ridx] > 0) pq.push({in_buf[ridx][in_pos[ridx]++], ridx});
                }
            }
            if (!out_buf.empty()) {
                write_block(fout, out_buf, out_buf.size());
                out_buf.clear();
            }
            fout.close();
            for (auto &fn : group) remove(fn.c_str());
            next.push_back(out_name);
        }
        cur.swap(next);
        ++pass;
    }
    return cur;
}

void ext_aridad_mergesort(const string &in_fname, const string &out_fname, size_t M_bytes, size_t d) {
    read_count = write_count = 0;
    auto runs = create_initial_runs(in_fname, M_bytes);
    size_t actual_runs = runs.size();
    if (d > actual_runs) {
        cerr << "MS: Aridad d=" << d << " > runs=" << actual_runs << ", capando a " << actual_runs << "\n";
        d = actual_runs;
    }
    auto final_runs = k_way_merge(runs, d, M_bytes);
    if (!final_runs.empty()) rename(final_runs[0].c_str(), out_fname.c_str());
    cout << "MS: Lecturas de bloques: " << read_count << endl;
    cout << "MS: Escrituras de bloques: " << write_count << endl;
}
