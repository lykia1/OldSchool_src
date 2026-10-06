using Microsoft.Web.WebView2.Core;
using System.Diagnostics;
using System.IO;
using System.Net.NetworkInformation;
using System.Text.Json;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace AceTRLauncher;

public partial class MainWindow : Window
{
    private readonly LauncherConfig _config;
    private readonly DispatcherTimer _timer = new() { Interval = TimeSpan.FromSeconds(3) };

    public MainWindow()
    {
        InitializeComponent();
        _config = LoadConfig();
        LoadSkin();
        RestoreRememberedAccount();

        _timer.Tick += async (_, _) => await RefreshServerStateAsync();
        _timer.Start();
        Loaded += async (_, _) => await RefreshServerStateAsync();
    }

    private static LauncherConfig LoadConfig()
    {
        try
        {
            var path = Path.Combine(AppContext.BaseDirectory, "launcher.json");
            if (!File.Exists(path)) return new LauncherConfig();
            return JsonSerializer.Deserialize<LauncherConfig>(File.ReadAllText(path),
                new JsonSerializerOptions { PropertyNameCaseInsensitive = true }) ?? new LauncherConfig();
        }
        catch { return new LauncherConfig(); }
    }

    private void LoadSkin()
    {
        var skinPath = Path.Combine(AppContext.BaseDirectory, "Assets", "launcher_master.jpg");
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
            var path = Path.Combine(AppContext.BaseDirectory, "remembered-account.txt");
            if (!File.Exists(path)) return;
            var remembered = File.ReadAllText(path).Trim();
            if (remembered.Length == 0) return;
            AccountBox.Text = remembered;
            RememberCheck.IsChecked = true;
        }
        catch { }
    }

    private async Task RefreshServerStateAsync()
    {
        try
        {
            var host = new Uri(_config.HomeUrl).Host;
            using var ping = new Ping();
            var result = await ping.SendPingAsync(host, 1200);
            if (result.Status == IPStatus.Success)
            {
                ServerStateText.Text = "ÇEVRİMİÇİ";
                ServerStateText.Foreground = System.Windows.Media.Brushes.LawnGreen;
                PingText.Text = $"Ping: {result.RoundtripTime} ms";
            }
            else throw new InvalidOperationException();
        }
        catch
        {
            ServerStateText.Text = "BAĞLANTI BEKLENİYOR";
            ServerStateText.Foreground = System.Windows.Media.Brushes.Orange;
            PingText.Text = "Ping: -- ms";
        }
    }

    private async Task ShowWebAsync(string url)
    {
        if (string.IsNullOrWhiteSpace(url))
        {
            ShowToast("Bu bağlantı launcher.json içinde tanımlanmamış.");
            return;
        }

        WebHostBorder.Visibility = Visibility.Visible;
        try
        {
            await WebView.EnsureCoreWebView2Async();
            WebView.CoreWebView2.Settings.AreDefaultContextMenusEnabled = false;
            WebView.CoreWebView2.Settings.AreDevToolsEnabled = false;
            WebView.Source = new Uri(url);
        }
        catch (Exception ex)
        {
            ShowToast("WebView2 açılamadı: " + ex.Message);
        }
    }

    private async void Home_Click(object sender, RoutedEventArgs e) => await ShowWebAsync(_config.HomeUrl);
    private void Discord_Click(object sender, RoutedEventArgs e) => OpenUrl(_config.DiscordUrl);
    private void Facebook_Click(object sender, RoutedEventArgs e) => OpenUrl(_config.FacebookUrl);
    private void Support_Click(object sender, RoutedEventArgs e) => OpenUrl(_config.SupportUrl);
    private void CloseWeb_Click(object sender, RoutedEventArgs e) => WebHostBorder.Visibility = Visibility.Collapsed;

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
            var rememberPath = Path.Combine(AppContext.BaseDirectory, "remembered-account.txt");
            if (RememberCheck.IsChecked == true)
                File.WriteAllText(rememberPath, account);
            else if (File.Exists(rememberPath))
                File.Delete(rememberPath);
        }
        catch { }

        var backend = Path.IsPathRooted(_config.BackendExecutable)
            ? _config.BackendExecutable
            : Path.Combine(AppContext.BaseDirectory, _config.BackendExecutable);

        if (!File.Exists(backend))
        {
            ShowToast("Oyun backend'i bulunamadı. launcher.json içindeki BackendExecutable yolunu ayarla.");
            return;
        }

        var psi = new ProcessStartInfo(backend)
        {
            UseShellExecute = false,
            WorkingDirectory = Path.GetDirectoryName(backend) ?? AppContext.BaseDirectory
        };

        // Credentials never appear in the command line.
        psi.Environment["ACETR_MODERN_LAUNCHER"] = "1";
        psi.Environment["ACETR_ACCOUNT"] = account;
        psi.Environment["ACETR_PASSWORD"] = password;
        psi.Environment["ACETR_REMEMBER"] = RememberCheck.IsChecked == true ? "1" : "0";
        psi.Environment["ACETR_64BIT"] = Bit64Check.IsChecked == true ? "1" : "0";
        psi.Environment["ACETR_WINDOWED"] = WindowedCheck.IsChecked == true ? "1" : "0";

        try
        {
            Process.Start(psi);
            PasswordBox.Clear();
            ShowToast("Oyun başlatma servisine aktarılıyor...");
        }
        catch (Exception ex)
        {
            ShowToast("Başlatma hatası: " + ex.Message);
        }
    }

    private void Settings_Click(object sender, RoutedEventArgs e) =>
        ShowToast("Ayarlar paneli bir sonraki adımda oyun çözünürlüğü ve kalite seçeneklerine bağlanacak.");

    private void Header_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
        if (e.ButtonState == MouseButtonState.Pressed) DragMove();
    }

    private void Minimize_Click(object sender, RoutedEventArgs e) => WindowState = WindowState.Minimized;
    private void Close_Click(object sender, RoutedEventArgs e) => Close();

    private void ShowToast(string text)
    {
        ToastText.Text = text;
        Toast.Visibility = Visibility.Visible;
        var timer = new DispatcherTimer { Interval = TimeSpan.FromSeconds(4) };
        timer.Tick += (_, _) => { Toast.Visibility = Visibility.Collapsed; timer.Stop(); };
        timer.Start();
    }
}
