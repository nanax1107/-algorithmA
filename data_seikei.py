def pprint_snap_for_c(input_file, output_file, is_directed=False):
    # 飛び飛びのIDを 0 〜 N-1 の連番にマッピングするための辞書
    id_map = {}
    next_id = 0
    edges = []
    
    # 1回目の読み込み: IDの割り振りとエッジの収集
    with open(input_file, 'r') as f:
        for line in f:
            # コメント行や空行をスキップ
            if line.startswith('#') or not line.strip():
                continue
            
            u_raw, v_raw = line.split()[:2]
            
            # 未登録のIDがあれば新しい連番を割り当てる
            if u_raw not in id_map:
                id_map[u_raw] = next_id
                next_id += 1
            if v_raw not in id_map:
                id_map[v_raw] = next_id
                next_id += 1
                
            edges.append((id_map[u_raw], id_map[v_raw]))
            
    # 2回目の書き出し: C言語がそのまま読める形式にする
    with open(output_file, 'w') as f_out:
        # 1行目に「実際のノード数」を書き込む
        f_out.write(f"{next_id}\n")
        
        # 2行目以降に「始点 終点 重み」を書き込む
        for u, v in edges:
            f_out.write(f"{u} {v} 1\n")
            
    print(f"変換完了: {input_file} -> {output_file}")
    print(f"実際のノード数(N): {next_id}, エッジ数: {len(edges)}")

# --- 実行例 ---
# Facebook用（無向グラフとして出力）
pprint_snap_for_c("facebook_combined.txt", "fb_for_c.txt", is_directed=False)

# Twitter用（有向グラフとして出力）
pprint_snap_for_c("twitter_combined.txt", "twitter_for_c.txt", is_directed=True)