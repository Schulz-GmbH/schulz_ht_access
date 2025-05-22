package de.schulz.hygiene.hs_access;

import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiNetworkSpecifier;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.webkit.ServiceWorkerController;
import android.webkit.ServiceWorkerWebSettings;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.EditText;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.DialogInterface;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.webkit.ServiceWorkerClient;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

/*
public class MainActivity extends AppCompatActivity {
    private static final String TARGET_SSID = "Schulz GmbH - HS-Access";
    private static final String PWA_URL     = "http://192.168.5.1";
    private WebView webView;

    private EditText ipAddressEditText;
    private Button connectButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Service Worker im WebView erlauben (Android 7.0+, API 24+)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            ServiceWorkerController swController = ServiceWorkerController.getInstance();
            ServiceWorkerWebSettings swSettings = swController.getServiceWorkerWebSettings();
            swSettings.setAllowContentAccess(true);
            swSettings.setAllowFileAccess(true);
        }

        // Debugging im WebView (nur im Debug‑Build empfohlen)
        WebView.setWebContentsDebuggingEnabled(true);

        setContentView(R.layout.activity_main);

        // Komponenten initialisieren
        webView = findViewById(R.id.webview);
        ipAddressEditText = findViewById(R.id.ip_address_edit_text);
        connectButton = findViewById(R.id.connect_button);

        // WebView konfigurieren
        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setDomStorageEnabled(true);
        webSettings.setDatabaseEnabled(true);
        webSettings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        webSettings.setCacheMode(WebSettings.LOAD_DEFAULT);

        // Wichtig: WebChromeClient registrieren, damit JS‐Alert funktioniert
        webView.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onJsAlert(WebView view, String url, String message, JsResult result) {
                // Baue und zeige einen nativen AlertDialog
                new AlertDialog.Builder(MainActivity.this)
                        .setTitle("Hinweis")
                        .setMessage(message)
                        .setPositiveButton(android.R.string.ok, (dialog, which) -> {
                            result.confirm();
                        })
                        .setCancelable(false)
                        .show();
                return true; // wir haben den Alert selbst behandelt
            }
        });

        webView.setWebViewClient(new WebViewClient());

        // Button‑Listener: URL laden und Cache‑Modus anpassen
        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String ip = ipAddressEditText.getText().toString().trim();
                if (ip.isEmpty()) return;

                if (!ip.startsWith("http://") && !ip.startsWith("https://")) {
                    ip = "http://" + ip;
                }
                Log.d("MainActivity", "Loading URL: " + ip);

                // Offline‑Check, dann Cache‑Modus setzen
                if (!isNetworkAvailable()) {
                    webSettings.setCacheMode(WebSettings.LOAD_CACHE_ELSE_NETWORK);
                    Log.d("MainActivity", "Offline: using cache");
                } else {
                    webSettings.setCacheMode(WebSettings.LOAD_DEFAULT);
                    Log.d("MainActivity", "Online: normal mode");
                }

                webView.loadUrl(ip);
            }
        });
    }

    @Override
    public void onBackPressed() {
        if (webView.canGoBack()) {
            webView.goBack();
        } else {
            super.onBackPressed();
        }
    }

    /** Prüft, ob eine Netzwerkverbindung verfügbar ist. *
    private boolean isNetworkAvailable() {
        ConnectivityManager cm = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo ni = (cm != null) ? cm.getActiveNetworkInfo() : null;
        return ni != null && ni.isConnected();
    }
}*/



public class MainActivity extends AppCompatActivity {

    private static final String TARGET_SSID = "Schulz GmbH - HS-Access";
    private static final String TARGET_PASSPHRASE = "12345678";
    private static final String PWA_URL     = "http://192.168.5.1";
    private static final int    RC_PERMISSIONS    = 1001;

    private WebView webView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        webView = findViewById(R.id.webview);

        checkAndRequestPermissions();
    }

    private void checkAndRequestPermissions() {
        String[] perms = new String[]{
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.CHANGE_NETWORK_STATE
        };

        boolean missing = false;
        for (String p : perms) {
            if (ContextCompat.checkSelfPermission(this, p)
                    != android.content.pm.PackageManager.PERMISSION_GRANTED) {
                missing = true;
                break;
            }
        }

        if (missing) {
            ActivityCompat.requestPermissions(this, perms, RC_PERMISSIONS);
        } else {
            pruefeWlanUndStarte();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == RC_PERMISSIONS) {
            boolean grantedAll = true;
            for (int res : grantResults) {
                if (res != android.content.pm.PackageManager.PERMISSION_GRANTED) {
                    grantedAll = false;
                    break;
                }
            }
            if (grantedAll) {
                pruefeWlanUndStarte();
            } else {
                new AlertDialog.Builder(this)
                        .setTitle("Berechtigung benötigt")
                        .setMessage("Ohne Netzwerk-Berechtigung kann die App das WLAN nicht verbinden.")
                        .setPositiveButton("Einstellungen öffnen", (d, w) -> {
                            Intent intent = new Intent(
                                    Settings.ACTION_APPLICATION_DETAILS_SETTINGS,
                                    Uri.parse("package:" + getPackageName()));
                            startActivity(intent);
                            finish();
                        })
                        .setNegativeButton("Beenden", (d, w) -> finish())
                        .setCancelable(false)
                        .show();
            }
        }
    }

    private void pruefeWlanUndStarte() {
        if (istMitZielWlanVerbunden()) {
            starteWebView();
        } else {
            zeigeVerbindungsDialog();
        }
    }

    private boolean istMitZielWlanVerbunden() {
        WifiManager wm = (WifiManager) getApplicationContext()
                .getSystemService(Context.WIFI_SERVICE);
        if (wm == null || !wm.isWifiEnabled()) return false;
        WifiInfo info = wm.getConnectionInfo();
        String ssid = info.getSSID();
        if (ssid != null && ssid.startsWith("\"") && ssid.endsWith("\"")) {
            ssid = ssid.substring(1, ssid.length() - 1);
        }
        return TARGET_SSID.equals(ssid);
    }

    private void zeigeVerbindungsDialog() {
        new AlertDialog.Builder(this)
                .setTitle("WLAN-Verbindung erforderlich")
                .setMessage("Bitte verbinden Sie sich mit dem WLAN \"" + TARGET_SSID + "\".")
                .setPositiveButton("Jetzt Verbinden", (dialog, which) -> verbindeMitWlan())
                .setNegativeButton("Abbrechen",       (dialog, which) -> starteWebView())
                .setCancelable(false)
                .show();
    }

    @SuppressWarnings("deprecation") // für WifiConfiguration (API < Q)
    private void verbindeMitWlan() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.CHANGE_NETWORK_STATE)
                != android.content.pm.PackageManager.PERMISSION_GRANTED) {
            checkAndRequestPermissions();
            return;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            WifiNetworkSpecifier specifier = new WifiNetworkSpecifier.Builder()
                    .setSsid(TARGET_SSID)
                    .setWpa2Passphrase(TARGET_PASSPHRASE)
                    .build();

            NetworkRequest request = new NetworkRequest.Builder()
                    .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
                    .setNetworkSpecifier(specifier)
                    .build();

            ConnectivityManager cm = (ConnectivityManager)
                    getSystemService(Context.CONNECTIVITY_SERVICE);

            cm.requestNetwork(request, new ConnectivityManager.NetworkCallback() {
                @Override
                public void onAvailable(@NonNull Network network) {
                    cm.bindProcessToNetwork(network);
                    runOnUiThread(() -> starteWebView());
                }
            });
        } else {
            WifiManager wm = (WifiManager) getApplicationContext()
                    .getSystemService(Context.WIFI_SERVICE);
            WifiConfiguration cfg = new WifiConfiguration();
            cfg.SSID = String.format("\"%s\"", TARGET_SSID);
            cfg.preSharedKey = String.format("\"%s\"", TARGET_PASSPHRASE);
            int netId = wm.addNetwork(cfg);
            wm.enableNetwork(netId, true);
            wm.reconnect();
            webView.postDelayed(this::starteWebView, 3000);
        }
    }

    private void starteWebView() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            ServiceWorkerController swc = ServiceWorkerController.getInstance();
            ServiceWorkerWebSettings sws = swc.getServiceWorkerWebSettings();
            sws.setAllowContentAccess(true);
            sws.setAllowFileAccess(true);
            swc.setServiceWorkerClient(new ServiceWorkerClient() {});
        }

        WebSettings ws = webView.getSettings();
        ws.setJavaScriptEnabled(true);
        ws.setDomStorageEnabled(true);
        ws.setAllowFileAccess(true);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            ws.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        }
        webView.setWebViewClient(new WebViewClient());
        webView.setWebChromeClient(new WebChromeClient());
        webView.loadUrl(PWA_URL);
    }

    @Override
    public void onBackPressed() {
        if (webView.canGoBack()) {
            webView.goBack();
        } else {
            super.onBackPressed();
        }
    }
}









