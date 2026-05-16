using System.Runtime.InteropServices;

using KccimHandle = nint;

var failures = 0;
var ctx = Native.kccim_init();

if (ctx == 0)
{
    Console.Error.WriteLine("kccim_init failed");
    return 1;
}

try
{
    PutKeys(ctx, "rksk");
    failures += ExpectText("basic composition", Native.GetString(ctx), "가나");

    Native.kccim_commit(ctx);
    Native.kccim_set_korean(ctx, 0);
    PutKeys(ctx, " ABC");
    failures += ExpectText("english mode", Native.GetString(ctx), "가나 ABC");

    Native.kccim_set_korean(ctx, 1);
    PutKeys(ctx, "qktk");
    failures += ExpectText("jong split before vowel", Native.GetString(ctx), "가나 ABC바사");
}
finally
{
    Native.kccim_free(ctx);
}

return failures == 0 ? 0 : 1;

static void PutKeys(KccimHandle ctx, string keys)
{
    foreach (var key in keys)
    {
        Native.kccim_put_key(ctx, key);
    }
}

static int ExpectText(string name, string actual, string expected)
{
    if (actual == expected)
    {
        Console.WriteLine($"PASS {name}: {actual}");
        return 0;
    }

    Console.Error.WriteLine($"FAIL {name}: expected [{expected}], got [{actual}]");
    return 1;
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
    internal static partial void kccim_commit(KccimHandle ctx);

    [LibraryImport("kccim")]
    internal static partial void kccim_set_korean(KccimHandle ctx, int isKorean);

    internal static string GetString(KccimHandle ctx)
    {
        return Marshal.PtrToStringUni(kccim_get_str(ctx)) ?? string.Empty;
    }
}
