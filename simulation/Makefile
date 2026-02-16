.PHONY: all s123 s4 s5 s6 s7 s8 s9 s10 s11 s12 s13 s14 s15 s16 s17 s18 s19 s20 s21 s22 s23 s24 s25 s26 clean

all: s123 s4 s5 s6 s7 s8 s9 s10 s11 s12 s13 s14 s15 s16 s17 s18 s19 s20 s21 s22 s23 s24 s25 s26

# Scenarios 1-3: Main Proofs (Perfect Storm, Entropy, Burst)
s123:
	@echo "\n🚀 Running Scenarios 1-3 (Main Proofs)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_main

# Scenario 4: chrono32y Encoding
s4:
	@echo "\n🚀 Running Scenario 4 (chrono32y)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_chrono32y

# Scenario 5: Mode C Routing
s5:
	@echo "\n🚀 Running Scenario 5 (Mode C)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_mode_c

# Scenario 6: Mode B Burst
s6:
	@echo "\n🚀 Running Scenario 6 (Mode B)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_mode_b

# Scenario 7: Rollback
s7:
	@echo "\n🚀 Running Scenario 7 (Rollback)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_rollback

# Scenario 8: Thread Contention
s8:
	@echo "\n🚀 Running Scenario 8 (Contention)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_contention

# Scenario 9: chrono32y Comparative Batches
s9:
	@echo "\n🚀 Running Scenario 9 (Comparison)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_chrono32y_batches

# Scenario 10: chrono32y Mode B
s10:
	@echo "\n🚀 Running Scenario 10 (Mode B Weyl)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_chrono32y_mode_b

# Scenario 11: Register Performance (64 vs 128)
s11:
	@echo "\n🚀 Running Scenario 11 (Performance Advantage)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_performance

# Scenario 12: Bit Audit
s12:
	@echo "\n🚀 Running Scenario 12 (Bit Audit)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_bit_splits

# Scenario 13: Sortability
s13:
	@echo "\n🚀 Running Scenario 13 (Sortability)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_sortability

# Scenario 14: Mode C Spin Wait
s14:
	@echo "\n🚀 Running Scenario 14 (Mode C Wait)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_mode_c_wait

# Scenario 15: Multi-Epoch Coexistence
s15:
	@echo "\n🚀 Running Scenario 15 (Epoch Sorting)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_epoch

# Scenario 16: Birthday Bound Accuracy
s16:
	@echo "\n🚀 Running Scenario 16 (Birthday Limits)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_birthday_limits

# Scenario 17: Index Locality
s17:
	@echo "\n🚀 Running Scenario 17 (Index Locality)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_locality

# Scenario 18: SQL Parity
s18:
	@echo "\n🚀 Running Scenario 18 (SQL Parity)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_sql_parity

# Scenario 19: Multiplier Avalanche
s19:
	@echo "\n🚀 Running Scenario 19 (Avalanche Test)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_avalanche

# Scenario 20: High-Frequency Risk
s20:
	@echo "\n🚀 Running Scenario 20 (High-Freq Safety)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_uncoordinated_high_freq

# Scenario 21: Variant Mixing Hazard
s21:
	@echo "\n🚀 Running Scenario 21 (Variant Mix)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_variant_mix

# Scenario 22: Causal Jitter (Clock Skew)
s22:
	@echo "\n🚀 Running Scenario 22 (Causal Jitter)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_causal_jitter

# Scenario 23: Boundary Value Analysis
s23:
	@echo "\n🚀 Running Scenario 23 (Boundary Analysis)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_boundary_values

# Scenario 24: Monotonicity Violation Audit
s24:
	@echo "\n🚀 Running Scenario 24 (Monotonicity Audit)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_monotonicity

# Scenario 25: chrono32y Performance
s25:
	@echo "\n🚀 Running Scenario 25 (chrono32y Performance)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_chrono32y_performance

# Scenario 26: Shard Routing Efficiency
s26:
	@echo "\n🚀 Running Scenario 26 (Routing Efficiency)..."
	@cd chrono_sim && cargo run --quiet --release --bin proof_routing_efficiency

# --- Visualizations ---

# Generate Simulation Data (CSV)
viz-data:
	@echo "\n🚀 Generating Deep Simulation Data for Plots..."
	@cd chrono_sim && cargo run --quiet --release --bin generate_viz_data
	@mkdir -p data
	@cp chrono_sim/simulation/data/*.csv data/

# Render Plots (PNG)
viz-plot:
	@echo "\n🚀 Rendering Empirical Graphs..."
	@mkdir -p plots
	@if [ ! -d "viz_venv" ]; then \
		echo "Creating virtual environment..."; \
		python3 -m venv viz_venv; \
		. viz_venv/bin/activate && pip install --quiet matplotlib pandas; \
	fi
	@. viz_venv/bin/activate && python3 plot_viz.py

# Full Visualization Workflow
viz: viz-data viz-plot
	@echo "\n✅ Visualizations generated in simulation/plots/"

# Extra: Unsigned Safety (Year 255)
unsigned_safety:
	@cd chrono_sim && cargo run --quiet --release --bin proof_unsigned_safety

clean:
	@cd chrono_sim && cargo clean
	rm -rf data plots viz_venv
