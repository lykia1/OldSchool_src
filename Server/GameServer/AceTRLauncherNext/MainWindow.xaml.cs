using System;
using System.Diagnostics;
using System.IO;
using System.Net.NetworkInformation;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace AceTRLauncher
{
    public partial class MainWindow : Window
    {
        private readonly LauncherConfig _config;
        private readonly DispatcherTimer _timer = new DispatcherTimer();

        public MainWindow()
        {
            InitializeComponent();

            _config = LauncherConfig.Load(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "launcher.ini"));
            LoadSkin();
            RestoreRememberedAccount();

            _timer.Interval = TimeSpan.FromSeconds(3);
            _timer.Tick += delegate { RefreshServerState(); };
            _timer.Start();
            Loaded += delegate { RefreshServerState(); };
        }

        private void LoadSkin()
        {
            var skinPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Assets", "launcher_master.jpg");
            if (!File.Exists(skinPath))
            {
                ShowToast("Assets\\launcher_master.jpg bulunamadı.");
                return;
            }

            var image = new BitmapImage();
            image.BeginInit();
            image.CacheOption = BitmapCacheOption.OnLoad;
            image.UriSource = new Uri(skinPath, UriKind.Absolute);
            image.EndInit();
            image.Freeze();
            SkinImage.Source = image;
        }

        private void RestoreRememberedAccount()
        {
            try
            {
                var path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "remembered-account.txt");
                if (!File.Exists(path)) return;
                var remembered = File.ReadAllText(path).Trim();
                if (remembered.Length == 0) return;
                AccountBox.Text = remembered;
                RememberCheck.IsChecked = true;
            }
            catch { }
        }

        private async void RefreshServerState()
        {
            try
            {
                var host = new Uri(_config.HomeUrl).Host;
                using (var ping = new Ping())
                {
                    var result = await ping.SendPingAsync(host, 1200);
                    if (result.Status == IPStatus.Success)
                    {
                        ServerStateText.Text = "ÇEVRİMİÇİ";
                        ServerStateText.Foreground = Brushes.LawnGreen;
                        PingText.Text = "Ping: " + result.RoundtripTime + " ms";
                        return;
                    }
                }
            }
            catch { }

            ServerStateText.Text = "BAĞLANTI BEKLENİYOR";
            ServerStateText.Foreground = Brushes.Orange;
            PingText.Text = "Ping: -- ms";
        }

        private void ShowWeb(string url)
        {
            if (string.IsNullOrWhiteSpace(url))
            {
                ShowToast("Bu bağlantı launcher.ini içinde tanımlanmamış.");
                return;
            }

            try
            {
                WebHostBorder.Visibility = Visibility.Visible;
                WebView.Navigate(new Uri(url));
            }
            catch (Exception ex)
            {
                ShowToast("Web sayfası açılamadı: " + ex.Message);
            }
        }

        private void Home_Click(object sender, RoutedEventArgs e) { ShowWeb(_config.HomeUrl); }
        private void Discord_Click(object sender, RoutedEventArgs e) { OpenUrl(_config.DiscordUrl); }
        private void Facebook_Click(object sender, RoutedEventArgs e) { OpenUrl(_config.FacebookUrl); }
        private void Support_Click(object sender, RoutedEventArgs e) { OpenUrl(_config.SupportUrl); }
        private void CloseWeb_Click(object sender, RoutedEventArgs e) { WebHostBorder.Visibility = Visibility.Collapsed; }

        private static void OpenUrl(string url)
        {
            if (string.IsNullOrWhiteSpace(url)) return;
            Process.Start(new ProcessStartInfo(url) { UseShellExecute = true });
        }

        private void Login_Click(object sender, RoutedEventArgs e)
        {
            var account = AccountBox.Text.Trim();
            var password = PasswordBox.Password;

            if (string.IsNullOrWhiteSpace(account) || string.IsNullOrWhiteSpace(password))
            {
                ShowToast("Kullanıcı adı ve şifre gerekli.");
                return;
            }

            try
            {
                var rememberPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "remembered-account.txt");
                if (RememberCheck.IsChecked == true) File.WriteAllText(rememberPath, account);
                else if (File.Exists(rememberPath)) File.Delete(rememberPath);
            }
            catch { }

            var backend = Path.IsPathRooted(_config.BackendExecutable)
                ? _config.BackendExecutable
                : Path.Combine(AppDomain.CurrentDomain.BaseDirectory, _config.BackendExecutable);

            if (!File.Exists(backend))
            {
                ShowToast("AtumLauncher backend bulunamadı. launcher.ini içindeki BackendExecutable yolunu ayarla.");
                return;
            }

            var psi = new ProcessStartInfo();
            psi.FileName = backend;
            psi.UseShellExecute = false;
            psi.WorkingDirectory = Path.GetDirectoryName(backend) ?? AppDomain.CurrentDomain.BaseDirectory;
            psi.EnvironmentVariables["ACETR_MODERN_LAUNCHER"] = "1";
            psi.EnvironmentVariables["ACETR_ACCOUNT"] = account;
            psi.EnvironmentVariables["ACETR_PASSWORD"] = password;
            psi.EnvironmentVariables["ACETR_REMEMBER"] = RememberCheck.IsChecked == true ? "1" : "0";
            psi.EnvironmentVariables["ACETR_64BIT"] = Bit64Check.IsChecked == true ? "1" : "0";
            psi.EnvironmentVariables["ACETR_WINDOWED"] = WindowedCheck.IsChecked == true ? "1" : "0";

            try
            {
                Process.Start(psi);
                PasswordBox.Clear();
                ShowToast("Giriş doğrulanıyor...");
            }
            catch (Exception ex)
            {
                ShowToast("Başlatma hatası: " + ex.Message);
            }
        }

        private void Settings_Click(object sender, RoutedEventArgs e)
        {
            ShowToast("Ayarlar paneli oyun seçeneklerine bağlanacak.");
        }

        private void Header_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ButtonState == MouseButtonState.Pressed) DragMove();
        }

        private void Minimize_Click(object sender, RoutedEventArgs e) { WindowState = WindowState.Minimized; }
        private void Close_Click(object sender, RoutedEventArgs e) { Close(); }

        private void ShowToast(string text)
        {
            ToastText.Text = text;
            Toast.Visibility = Visibility.Visible;
            var timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(4);
            timer.Tick += delegate
            {
                Toast.Visibility = Visibility.Collapsed;
                timer.Stop();
            };
            timer.Start();
        }
    }
}
