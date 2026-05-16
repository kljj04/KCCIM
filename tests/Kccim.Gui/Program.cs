using System.Runtime.InteropServices;

namespace Kccim.Gui;

using KccimHandle = nint;

internal static class Program
{
    [STAThread]
    private static void Main()
    {
        ApplicationConfiguration.Initialize();
        Application.Run(new MainForm());
    }
}

internal sealed class MainForm : Form
{
    private KccimHandle _ctx;
    private readonly RichTextBox _editor = new();
    private readonly Label _mode = new();
    private readonly Label _composition = new();
    private bool _isKorean = true;
    private int? _compositionStart;
    private int _renderedCompositionLength;
    private bool _updatingEditor;

    public MainForm()
    {
        ResetContext();

        Text = "KCCIM Manual Input Test";
        Width = 820;
        Height = 560;
        StartPosition = FormStartPosition.CenterScreen;
        Font = new Font("Segoe UI", 10F);

        var panel = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            RowCount = 3,
            ColumnCount = 1,
            Padding = new Padding(12),
        };
        panel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
        panel.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
        panel.RowStyles.Add(new RowStyle(SizeType.AutoSize));

        var toolbar = new FlowLayoutPanel
        {
            Dock = DockStyle.Fill,
            AutoSize = true,
            FlowDirection = FlowDirection.LeftToRight,
            WrapContents = true,
        };

        var toggle = new Button { Text = "Toggle Hangul", AutoSize = true, Margin = new Padding(0, 0, 8, 8) };
        toggle.Click += (_, _) => ToggleMode();

        var commit = new Button { Text = "Commit", AutoSize = true, Margin = new Padding(0, 0, 8, 8) };
        commit.Click += (_, _) => CommitComposition();

        var clear = new Button { Text = "Clear", AutoSize = true, Margin = new Padding(0, 0, 8, 8) };
        clear.Click += (_, _) =>
        {
            ResetContext();
            _editor.Clear();
            _compositionStart = null;
            _renderedCompositionLength = 0;
            RefreshStatus();
        };

        _mode.AutoSize = true;
        _mode.Margin = new Padding(12, 5, 8, 8);
        _composition.AutoSize = true;
        _composition.Margin = new Padding(12, 5, 8, 8);
        _composition.Padding = new Padding(6, 2, 6, 2);

        toolbar.Controls.Add(toggle);
        toolbar.Controls.Add(commit);
        toolbar.Controls.Add(clear);
        toolbar.Controls.Add(_mode);
        toolbar.Controls.Add(_composition);

        _editor.Dock = DockStyle.Fill;
        _editor.Multiline = true;
        _editor.ScrollBars = RichTextBoxScrollBars.Vertical;
        _editor.Font = new Font("Malgun Gothic", 16F);
        _editor.AcceptsTab = true;
        _editor.BackColor = Color.White;
        _editor.ImeMode = ImeMode.Disable;
        _editor.WordWrap = true;
        _editor.KeyDown += OnEditorKeyDown;
        _editor.SelectionChanged += OnEditorSelectionChanged;

        var hint = new Label
        {
            Dock = DockStyle.Fill,
            AutoSize = true,
            Text = "KCCIM handles keys directly; Windows IME is disabled for this editor.",
            ForeColor = SystemColors.GrayText,
            Padding = new Padding(0, 8, 0, 0),
        };

        panel.Controls.Add(toolbar, 0, 0);
        panel.Controls.Add(_editor, 0, 1);
        panel.Controls.Add(hint, 0, 2);
        Controls.Add(panel);

        RefreshStatus();
    }

    protected override void OnFormClosed(FormClosedEventArgs e)
    {
        if (_ctx != 0)
        {
            Native.kccim_free(_ctx);
            _ctx = 0;
        }
        base.OnFormClosed(e);
    }

    private void OnEditorKeyDown(object? sender, KeyEventArgs e)
    {
        if (e.KeyCode is Keys.HangulMode or Keys.RMenu)
        {
            ToggleMode();
            e.SuppressKeyPress = true;
            return;
        }

        var vkCode = ToInputCode(e);
        if (vkCode is null)
        {
            return;
        }

        ApplyKey(vkCode.Value);
        e.SuppressKeyPress = true;
    }

    private void OnEditorSelectionChanged(object? sender, EventArgs e)
    {
        if (_updatingEditor || _compositionStart is null)
        {
            return;
        }

        var start = _compositionStart.Value;
        var end = start + _renderedCompositionLength;
        var caret = _editor.SelectionStart;

        if (caret < start || caret > end || _editor.SelectionLength > 0)
        {
            CommitComposition();
        }
    }

    private static int? ToInputCode(KeyEventArgs e)
    {
        if (e.KeyCode == Keys.Back) return 0x08;
        if (e.KeyCode == Keys.Space) return 0x20;
        if (e.KeyCode == Keys.Enter) return 0x0D;

        if (e.KeyCode is >= Keys.A and <= Keys.Z)
        {
            var ch = (char)('A' + (e.KeyCode - Keys.A));
            return e.Shift ? ch : char.ToLowerInvariant(ch);
        }

        if (e.KeyCode is >= Keys.D0 and <= Keys.D9)
        {
            return '0' + (e.KeyCode - Keys.D0);
        }

        return null;
    }

    private void ApplyKey(int vkCode)
    {
        if (_compositionStart is null)
        {
            if (vkCode == 0x08)
            {
                DeleteBeforeCaret();
                RefreshStatus();
                return;
            }

            _compositionStart = _editor.SelectionStart;
            _renderedCompositionLength = 0;
            ResetContext();
            Native.kccim_set_korean(_ctx, _isKorean ? 1 : 0);
        }

        Native.kccim_put_key(_ctx, vkCode);
        RenderComposition();
    }

    private void RenderComposition()
    {
        var start = Math.Min(_compositionStart ?? _editor.SelectionStart, _editor.TextLength);
        var text = Native.GetString(_ctx);
        var oldLength = Math.Min(_renderedCompositionLength, _editor.TextLength - start);

        _updatingEditor = true;
        try
        {
            _editor.Select(start, oldLength);
            _editor.SelectedText = text;
            _renderedCompositionLength = text.Length;
            _editor.SelectionStart = start + text.Length;
            _editor.SelectionLength = 0;
        }
        finally
        {
            _updatingEditor = false;
        }

        RefreshStatus();
        HighlightComposition(start);
    }

    private void HighlightComposition(int segmentStart)
    {
        var info = Native.kccim_get_info(_ctx);

        var caret = _editor.SelectionStart;
        var selectionLength = _editor.SelectionLength;

        _updatingEditor = true;
        try
        {
            _editor.SelectAll();
            _editor.SelectionBackColor = _editor.BackColor;
            _editor.SelectionColor = _editor.ForeColor;

            if (info.CompLength > 0)
            {
                var compStart = Math.Max(segmentStart, segmentStart + _renderedCompositionLength - info.CompLength);
                _editor.Select(compStart, info.CompLength);
                _editor.SelectionBackColor = Color.FromArgb(255, 230, 120);
                _editor.SelectionColor = Color.FromArgb(30, 64, 175);
            }
            else if (_renderedCompositionLength > 0)
            {
                CommitComposition(resetOnly: true);
            }

            _editor.Select(Math.Min(caret, _editor.TextLength), selectionLength);
        }
        finally
        {
            _updatingEditor = false;
        }
    }

    private void CommitComposition(bool resetOnly = false)
    {
        _compositionStart = null;
        _renderedCompositionLength = 0;
        ResetContext();
        Native.kccim_set_korean(_ctx, _isKorean ? 1 : 0);
        if (!resetOnly)
        {
            RefreshStatus();
        }
    }

    private void ToggleMode()
    {
        CommitComposition();
        _isKorean = !_isKorean;
        Native.kccim_set_korean(_ctx, _isKorean ? 1 : 0);
        RefreshStatus();
    }

    private void RefreshStatus()
    {
        var info = _ctx == 0 ? default : Native.kccim_get_info(_ctx);
        var comp = Native.PtrToString(info.CompStr);
        _mode.Text = _isKorean ? "Mode: Korean" : "Mode: English";
        _composition.Text = info.CompLength > 0 ? $"IME COMPOSITION: {comp}" : "IME COMPOSITION: none";
        _composition.BackColor = info.CompLength > 0 ? Color.FromArgb(255, 230, 120) : SystemColors.Control;
        _composition.ForeColor = info.CompLength > 0 ? Color.FromArgb(30, 64, 175) : SystemColors.ControlText;
    }

    private void DeleteBeforeCaret()
    {
        var caret = _editor.SelectionStart;
        if (_editor.SelectionLength > 0)
        {
            _editor.SelectedText = string.Empty;
            return;
        }

        if (caret <= 0) return;

        _editor.Select(caret - 1, 1);
        _editor.SelectedText = string.Empty;
    }

    private void ResetContext()
    {
        if (_ctx != 0)
        {
            Native.kccim_free(_ctx);
        }

        _ctx = Native.kccim_init();
        if (_ctx == 0)
        {
            throw new InvalidOperationException("kccim_init failed.");
        }
    }
}

[StructLayout(LayoutKind.Sequential)]
internal readonly struct KccimInfo
{
    public readonly int IsKorean;
    public readonly int CompLength;
    public readonly int CompStatus;
    public readonly nint CompStr;
}

internal static partial class Native
{
    [LibraryImport("kccim")]
    internal static partial KccimHandle kccim_init();

    [LibraryImport("kccim")]
    internal static partial void kccim_free(KccimHandle ctx);

    [LibraryImport("kccim")]
    internal static partial int kccim_put_key(KccimHandle ctx, int vkCode);

    [LibraryImport("kccim")]
    private static partial nint kccim_get_str(KccimHandle ctx);

    [LibraryImport("kccim")]
    internal static partial KccimInfo kccim_get_info(KccimHandle ctx);

    [LibraryImport("kccim")]
    internal static partial void kccim_set_korean(KccimHandle ctx, int isKorean);

    internal static string GetString(KccimHandle ctx) => PtrToString(kccim_get_str(ctx));

    internal static string PtrToString(nint ptr) => Marshal.PtrToStringUni(ptr) ?? string.Empty;
}
