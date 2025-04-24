import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
import serial
import serial.tools.list_ports
import threading
import time
import re
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import datetime

class MicroVerESSController:
    def __init__(self, root):
        self.root = root
        self.root.title("MicroVer ESS Controller")
        self.root.geometry("900x700")
        self.root.resizable(True, True)
        self.root.configure(bg="#f0f0f0")
        
        # Değişkenler
        self.serial_port = None
        self.is_connected = False
        self.update_thread = None
        self.stop_thread = False
        self.data_buffer = ""
        self.voltage_history = {
            "time": [],
            "load": [],
            "charge": [],
            "bank_a": [],
            "bank_b": []
        }
        self.max_history_points = 100
        
        self.setup_ui()
    
    def setup_ui(self):
        # Ana frame'ler
        top_frame = tk.Frame(self.root, bg="#f0f0f0")
        top_frame.pack(fill=tk.X, padx=10, pady=5)
        
        left_frame = tk.Frame(self.root, bg="#f0f0f0")
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        right_frame = tk.Frame(self.root, bg="#f0f0f0")
        right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        # Port bağlantısı
        port_frame = tk.LabelFrame(top_frame, text="USB Bağlantısı", bg="#f0f0f0", padx=5, pady=5)
        port_frame.pack(side=tk.LEFT, fill=tk.X, expand=True)
        
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(port_frame, textvariable=self.port_var)
        self.port_combo.pack(side=tk.LEFT, padx=5, pady=5)
        
        refresh_btn = ttk.Button(port_frame, text="Yenile", command=self.refresh_ports)
        refresh_btn.pack(side=tk.LEFT, padx=5, pady=5)
        
        self.connect_btn = ttk.Button(port_frame, text="Bağlan", command=self.toggle_connection)
        self.connect_btn.pack(side=tk.LEFT, padx=5, pady=5)
        
        # Durum göstergeleri
        status_frame = tk.LabelFrame(left_frame, text="Sistem Durumu", bg="#f0f0f0", padx=5, pady=5)
        status_frame.pack(fill=tk.BOTH, expand=False, padx=5, pady=5)
        
        # Batarya seviyesi
        bat_frame = tk.Frame(status_frame, bg="#f0f0f0")
        bat_frame.pack(fill=tk.X, padx=5, pady=5)
        
        tk.Label(bat_frame, text="Batarya Seviyesi:", bg="#f0f0f0").pack(side=tk.LEFT, padx=5)
        self.battery_var = tk.StringVar(value="0%")
        tk.Label(bat_frame, textvariable=self.battery_var, bg="#f0f0f0", font=("Arial", 12, "bold")).pack(side=tk.LEFT, padx=5)
        
        self.battery_progress = ttk.Progressbar(bat_frame, orient="horizontal", length=200, mode="determinate")
        self.battery_progress.pack(side=tk.LEFT, padx=5)
        
        # Sistem durumu
        state_frame = tk.Frame(status_frame, bg="#f0f0f0")
        state_frame.pack(fill=tk.X, padx=5, pady=5)
        
        tk.Label(state_frame, text="Sistem Durumu:", bg="#f0f0f0").pack(side=tk.LEFT, padx=5)
        self.state_var = tk.StringVar(value="Bağlantı yok")
        tk.Label(state_frame, textvariable=self.state_var, bg="#f0f0f0", font=("Arial", 12, "bold")).pack(side=tk.LEFT, padx=5)
        
        # Hata durumu
        fault_frame = tk.Frame(status_frame, bg="#f0f0f0")
        fault_frame.pack(fill=tk.X, padx=5, pady=5)
        
        tk.Label(fault_frame, text="Hata Durumu:", bg="#f0f0f0").pack(side=tk.LEFT, padx=5)
        self.fault_var = tk.StringVar(value="Yok")
        tk.Label(fault_frame, textvariable=self.fault_var, bg="#f0f0f0", font=("Arial", 12, "bold")).pack(side=tk.LEFT, padx=5)
        
        # Voltaj ölçümleri
        voltage_frame = tk.LabelFrame(status_frame, text="Voltaj Ölçümleri", bg="#f0f0f0", padx=5, pady=5)
        voltage_frame.pack(fill=tk.X, padx=5, pady=5)
        
        # Yük voltajı
        v_load_frame = tk.Frame(voltage_frame, bg="#f0f0f0")
        v_load_frame.pack(fill=tk.X, padx=5, pady=2)
        tk.Label(v_load_frame, text="Yük Voltajı:", bg="#f0f0f0", width=15, anchor="w").pack(side=tk.LEFT, padx=5)
        self.v_load_var = tk.StringVar(value="0.0V")
        tk.Label(v_load_frame, textvariable=self.v_load_var, bg="#f0f0f0", width=10).pack(side=tk.LEFT, padx=5)
        
        # Şarj voltajı
        v_charge_frame = tk.Frame(voltage_frame, bg="#f0f0f0")
        v_charge_frame.pack(fill=tk.X, padx=5, pady=2)
        tk.Label(v_charge_frame, text="Şarj Voltajı:", bg="#f0f0f0", width=15, anchor="w").pack(side=tk.LEFT, padx=5)
        self.v_charge_var = tk.StringVar(value="0.0V")
        tk.Label(v_charge_frame, textvariable=self.v_charge_var, bg="#f0f0f0", width=10).pack(side=tk.LEFT, padx=5)
        
        # Bank A voltajı
        v_bank_a_frame = tk.Frame(voltage_frame, bg="#f0f0f0")
        v_bank_a_frame.pack(fill=tk.X, padx=5, pady=2)
        tk.Label(v_bank_a_frame, text="Bank A Voltajı:", bg="#f0f0f0", width=15, anchor="w").pack(side=tk.LEFT, padx=5)
        self.v_bank_a_var = tk.StringVar(value="0.0V")
        tk.Label(v_bank_a_frame, textvariable=self.v_bank_a_var, bg="#f0f0f0", width=10).pack(side=tk.LEFT, padx=5)
        
        # Bank B voltajı
        v_bank_b_frame = tk.Frame(voltage_frame, bg="#f0f0f0")
        v_bank_b_frame.pack(fill=tk.X, padx=5, pady=2)
        tk.Label(v_bank_b_frame, text="Bank B Voltajı:", bg="#f0f0f0", width=15, anchor="w").pack(side=tk.LEFT, padx=5)
        self.v_bank_b_var = tk.StringVar(value="0.0V")
        tk.Label(v_bank_b_frame, textvariable=self.v_bank_b_var, bg="#f0f0f0", width=10).pack(side=tk.LEFT, padx=5)
        
        # Kontrol paneli
        control_frame = tk.LabelFrame(left_frame, text="Kontrol Paneli", bg="#f0f0f0", padx=5, pady=5)
        control_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # LED kontrolleri
        led_frame = tk.LabelFrame(control_frame, text="LED Kontrol", bg="#f0f0f0", padx=5, pady=5)
        led_frame.pack(fill=tk.X, padx=5, pady=5)
        
        led_names = ["LED 0%", "LED 50%", "LED 100%", "LED Şarj", "LED Kontrol", "Uyarı LED"]
        self.led_vars = []
        
        for i, name in enumerate(led_names):
            var = tk.BooleanVar(value=False)
            self.led_vars.append(var)
            
            led_cb = ttk.Checkbutton(led_frame, text=name, variable=var, command=lambda idx=i: self.control_led(idx))
            led_cb.grid(row=i//3, column=i%3, padx=5, pady=2, sticky="w")
        
        # Şarj kontrolleri
        charge_frame = tk.LabelFrame(control_frame, text="Şarj Kontrol", bg="#f0f0f0", padx=5, pady=5)
        charge_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.charge_var = tk.IntVar(value=0)
        ttk.Radiobutton(charge_frame, text="Şarj Kapalı", variable=self.charge_var, value=0, command=self.control_charge).grid(row=0, column=0, padx=5, pady=2, sticky="w")
        ttk.Radiobutton(charge_frame, text="Normal Şarj", variable=self.charge_var, value=1, command=self.control_charge).grid(row=0, column=1, padx=5, pady=2, sticky="w")
        ttk.Radiobutton(charge_frame, text="Hızlı Şarj", variable=self.charge_var, value=2, command=self.control_charge).grid(row=0, column=2, padx=5, pady=2, sticky="w")
        
        # Güç çıkışı kontrolü
        power_frame = tk.LabelFrame(control_frame, text="Güç Çıkışı", bg="#f0f0f0", padx=5, pady=5)
        power_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.power_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(power_frame, text="Güç Çıkışı Etkin", variable=self.power_var, command=self.control_power).grid(row=0, column=0, padx=5, pady=2, sticky="w")
        
        # Mandal röle kontrolü
        relay_frame = tk.LabelFrame(control_frame, text="Mandal Röle Kontrol", bg="#f0f0f0", padx=5, pady=5)
        relay_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Button(relay_frame, text="Röle Set", command=lambda: self.control_relay(1)).grid(row=0, column=0, padx=5, pady=2)
        ttk.Button(relay_frame, text="Röle Reset", command=lambda: self.control_relay(2)).grid(row=0, column=1, padx=5, pady=2)
        ttk.Button(relay_frame, text="Röle Kapalı", command=lambda: self.control_relay(0)).grid(row=0, column=2, padx=5, pady=2)
        
        # Manuel komut gönderme
        cmd_frame = tk.LabelFrame(control_frame, text="Manuel Komut", bg="#f0f0f0", padx=5, pady=5)
        cmd_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.cmd_var = tk.StringVar()
        cmd_entry = ttk.Entry(cmd_frame, textvariable=self.cmd_var)
        cmd_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5, pady=5)
        
        send_btn = ttk.Button(cmd_frame, text="Gönder", command=self.send_command)
        send_btn.pack(side=tk.LEFT, padx=5, pady=5)
        
        # Durum güncellemesi
        update_btn = ttk.Button(control_frame, text="Durum Güncelle", command=lambda: self.send_command("S"))
        update_btn.pack(fill=tk.X, padx=5, pady=5)
        
        # Grafik ve log paneli
        graph_frame = tk.LabelFrame(right_frame, text="Voltaj Grafiği", bg="#f0f0f0", padx=5, pady=5)
        graph_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Matplotlib grafiği
        self.fig, self.ax = plt.subplots(figsize=(5, 3), dpi=100)
        self.canvas = FigureCanvasTkAgg(self.fig, master=graph_frame)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        self.ax.set_title("Voltaj Değişimi")
        self.ax.set_xlabel("Zaman")
        self.ax.set_ylabel("Voltaj (V)")
        self.ax.grid(True)
        
        # Log paneli
        log_frame = tk.LabelFrame(right_frame, text="Sistem Logu", bg="#f0f0f0", padx=5, pady=5)
        log_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.log_text = scrolledtext.ScrolledText(log_frame, wrap=tk.WORD, height=10)
        self.log_text.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # İlk port listesini yükle
        self.refresh_ports()
        
        # UI'ı devre dışı bırak (bağlantı yokken)
        self.toggle_ui_state(False)
    
    def refresh_ports(self):
        """Kullanılabilir seri portları listeler"""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combo['values'] = ports
        
        if ports and not self.port_var.get():
            self.port_var.set(ports[0])
    
    def toggle_connection(self):
        """Seri port bağlantısını açar/kapatır"""
        if self.is_connected:
            # Bağlantıyı kapat
            self.stop_thread = True
            if self.update_thread:
                self.update_thread.join()
            
            if self.serial_port and self.serial_port.is_open:
                self.serial_port.close()
            
            self.is_connected = False
            self.connect_btn.config(text="Bağlan")
            self.toggle_ui_state(False)
            self.log_message("Bağlantı kapatıldı")
        else:
            # Bağlantıyı aç
            port = self.port_var.get()
            if not port:
                messagebox.showerror("Hata", "Lütfen bir port seçin")
                return
            
            try:
                self.serial_port = serial.Serial(port, 115200, timeout=1)
                self.is_connected = True
                self.connect_btn.config(text="Bağlantıyı Kes")
                self.toggle_ui_state(True)
                self.log_message(f"Port {port} bağlantısı açıldı")
                
                # Durum güncelleme thread'ini başlat
                self.stop_thread = False
                self.update_thread = threading.Thread(target=self.update_status_loop)
                self.update_thread.daemon = True
                self.update_thread.start()
                
                # Başlangıçta status güncelle
                self.send_command("S")
                
            except Exception as e:
                messagebox.showerror("Bağlantı Hatası", str(e))
                self.log_message(f"Bağlantı hatası: {str(e)}")
    
    def toggle_ui_state(self, enable):
        """UI elemanlarını etkinleştirir/devre dışı bırakır"""
        state = tk.NORMAL if enable else tk.DISABLED
        
        # Kontrol panelindeki tüm butonları ve giriş alanlarını devre dışı bırak
        for widget in self.root.winfo_children():
            if isinstance(widget, tk.Frame):
                for child in widget.winfo_children():
                    if isinstance(child, (tk.LabelFrame, tk.Frame)):
                        for grandchild in child.winfo_children():
                            if isinstance(grandchild, (ttk.Button, ttk.Entry, ttk.Checkbutton, ttk.Radiobutton)):
                                grandchild.config(state=state)
                            elif isinstance(grandchild, tk.Frame):
                                for great_grandchild in grandchild.winfo_children():
                                    if isinstance(great_grandchild, (ttk.Button, ttk.Entry, ttk.Checkbutton, ttk.Radiobutton)):
                                        great_grandchild.config(state=state)
        
        # Port bağlantı butonlarını her zaman etkin tut
        self.connect_btn.config(state=tk.NORMAL)
        self.port_combo.config(state=tk.NORMAL)
    
    def update_status_loop(self):
        """Düzenli olarak durum güncelleme döngüsü"""
        while not self.stop_thread:
            if self.is_connected and self.serial_port and self.serial_port.is_open:
                try:
                    # Veri var mı kontrol et
                    if self.serial_port.in_waiting > 0:
                        data = self.serial_port.read(self.serial_port.in_waiting).decode('ascii', errors='ignore')
                        self.data_buffer += data
                        
                        # Tam satırları işle
                        lines = self.data_buffer.split('\r\n')
                        self.data_buffer = lines[-1]  # Son (tamamlanmamış) satırı buffer'da tut
                        
                        for line in lines[:-1]:  # Tamamlanmış tüm satırları işle
                            if line.strip():
                                self.log_message(f"< {line}")
                                self.parse_status(line)
                
                    # Her 2 saniyede bir durum güncelle
                    if time.time() - self.last_status_time > 2:
                        self.send_command("S")
                        self.last_status_time = time.time()
                
                except Exception as e:
                    self.log_message(f"Veri okuma hatası: {str(e)}")
                    break
            
            time.sleep(0.1)
        
        self.is_connected = False
        self.connect_btn.config(text="Bağlan")
    
    def parse_status(self, data):
        """Durum mesajını ayrıştırır"""
        if data.startswith("BAT:"):
            try:
                # Durum mesajını ayrıştır: BAT:75,STATE:1,FAULT:0,V1:123,V2:456,V3:789,V4:012
                parts = data.split(',')
                bat_match = re.search(r'BAT:(\d+)', data)
                state_match = re.search(r'STATE:(\d+)', data)
                fault_match = re.search(r'FAULT:(\d+)', data)
                v1_match = re.search(r'V1:(\d+)', data)
                v2_match = re.search(r'V2:(\d+)', data)
                v3_match = re.search(r'V3:(\d+)', data)
                v4_match = re.search(r'V4:(\d+)', data)
                
                if bat_match:
                    bat_level = int(bat_match.group(1))
                    self.battery_var.set(f"{bat_level}%")
                    self.battery_progress['value'] = bat_level
                
                if state_match:
                    state = int(state_match.group(1))
                    states = ["Beklemede", "Şarj Oluyor", "Deşarj Oluyor", "Hata"]
                    self.state_var.set(states[state] if state < len(states) else f"Bilinmeyen ({state})")
                
                if fault_match:
                    fault = int(fault_match.group(1))
                    if fault == 0:
                        self.fault_var.set("Yok")
                    else:
                        fault_str = ""
                        if fault & 0x01:
                            fault_str += "BLOCK_200A "
                        if fault & 0x02:
                            fault_str += "BLOCK_100A "
                        if fault & 0x04:
                            fault_str += "CHARGE "
                        if fault & 0x08:
                            fault_str += "FAST_CHARGE"
                        self.fault_var.set(fault_str)
                
                # Voltaj değerlerini işle
                adc_to_voltage = lambda x: round(x * 3.3 / 4095 * 5, 2)  # ADC değerinden voltaja dönüşüm (örnek)
                
                if v1_match:
                    v1 = int(v1_match.group(1))
                    voltage = adc_to_voltage(v1)
                    self.v_load_var.set(f"{voltage:.1f}V")
                    
                    # Geçmiş veriye ekle
                    current_time = datetime.datetime.now().strftime('%H:%M:%S')
                    self.voltage_history["time"].append(current_time)
                    self.voltage_history["load"].append(voltage)
                    
                    # Tarih listesini sınırla
                    if len(self.voltage_history["time"]) > self.max_history_points:
                        self.voltage_history["time"] = self.voltage_history["time"][-self.max_history_points:]
                        self.voltage_history["load"] = self.voltage_history["load"][-self.max_history_points:]
                
                if v2_match:
                    v2 = int(v2_match.group(1))
                    voltage = adc_to_voltage(v2)
                    self.v_charge_var.set(f"{voltage:.1f}V")
                    
                    self.voltage_history["charge"].append(voltage)
                    if len(self.voltage_history["charge"]) > self.max_history_points:
                        self.voltage_history["charge"] = self.voltage_history["charge"][-self.max_history_points:]
                
                if v3_match:
                    v3 = int(v3_match.group(1))
                    voltage = adc_to_voltage(v3)
                    self.v_bank_a_var.set(f"{voltage:.1f}V")
                    
                    self.voltage_history["bank_a"].append(voltage)
                    if len(self.voltage_history["bank_a"]) > self.max_history_points:
                        self.voltage_history["bank_a"] = self.voltage_history["bank_a"][-self.max_history_points:]
                
                if v4_match:
                    v4 = int(v4_match.group(1))
                    voltage = adc_to_voltage(v4)
                    self.v_bank_b_var.set(f"{voltage:.1f}V")
                    
                    self.voltage_history["bank_b"].append(voltage)
                    if len(self.voltage_history["bank_b"]) > self.max_history_points:
                        self.voltage_history["bank_b"] = self.voltage_history["bank_b"][-self.max_history_points:]
                
                # Grafiği güncelle
                self.update_voltage_graph()
                
            except Exception as e:
                self.log_message(f"Durum ayrıştırma hatası: {str(e)}")
    
    def update_voltage_graph(self):
        """Voltaj grafiğini günceller"""
        if len(self.voltage_history["time"]) > 1:
            self.ax.clear()
            self.ax.plot(self.voltage_history["time"], self.voltage_history["load"], label="Yük", marker='o', markersize=2)
            self.ax.plot(self.voltage_history["time"], self.voltage_history["charge"], label="Şarj", marker='s', markersize=2)
            self.ax.plot(self.voltage_history["time"], self.voltage_history["bank_a"], label="Bank A", marker='^', markersize=2)
            self.ax.plot(self.voltage_history["time"], self.voltage_history["bank_b"], label="Bank B", marker='x', markersize=2)
            
            self.ax.set_title("Voltaj Değişimi")
            self.ax.set_xlabel("Zaman")
            self.ax.set_ylabel("Voltaj (V)")
            self.ax.grid(True)
            self.ax.legend(loc='upper right')
            
            # X ekseni etiketlerini düzenle
            num_labels = 5  # Gösterilecek etiket sayısı
            indices = [i * (len(self.voltage_history["time"])-1) // (num_labels-1) for i in range(num_labels)]
            if indices and indices[-1] >= len(self.voltage_history["time"]):
                indices[-1] = len(self.voltage_history["time"]) - 1
            
            self.ax.set_xticks([self.voltage_history["time"][i] for i in indices if i < len(self.voltage_history["time"])])
            plt.setp(self.ax.get_xticklabels(), rotation=30, ha='right')
            
            self.canvas.draw()
    
    def control_led(self, index):
        """LED'leri kontrol eder"""
        state = 1 if self.led_vars[index].get() else 0
        cmd = f"L{index}{state}"
        self.send_command(cmd)
    
    def control_charge(self):
        """Şarj kontrolü"""
        cmd = f"C{self.charge_var.get()}"
        self.send_command(cmd)
    
    def control_power(self):
        """Güç çıkışı kontrolü"""
        state = 1 if self.power_var.get() else 0
        cmd = f"P{state}"
        self.send_command(cmd)
    
    def control_relay(self, state):
        """Mandal röle kontrolü"""
        cmd = f"R{state}"
        self.send_command(cmd)
    
    def send_command(self, cmd=None):
        """Komutu seri porta gönderir"""
        if not self.is_connected or not self.serial_port or not self.serial_port.is_open:
            messagebox.showerror("Hata", "Seri port bağlı değil")
            return
        
        if cmd is None:
            cmd = self.cmd_var.get()
            self.cmd_var.set("")  # Giriş alanını temizle
        
        if cmd:
            try:
                self.serial_port.write((cmd + '\r\n').encode('ascii'))
                self.log_message(f"> {cmd}")
                
                if cmd == "S":
                    self.last_status_time = time.time()
                
            except Exception as e:
                self.log_message(f"Komut gönderme hatası: {str(e)}")
    
    def log_message(self, message):
        """Log alanına mesaj ekler"""
        timestamp = datetime.datetime.now().strftime('[%H:%M:%S] ')
        self.log_text.insert(tk.END, timestamp + message + '\n')
        self.log_text.see(tk.END)  # Sona kaydır

if __name__ == "__main__":
    root = tk.Tk()
    app = MicroVerESSController(root)
    # Global değişken ekle
    app.last_status_time = 0
    root.mainloop()