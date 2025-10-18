<!DOCTYPE html>
<html lang="en" class="dark">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>List of Releases</title>
    <script src="tailwind.js"></script>
    <script>
        tailwind.config = {
            darkMode: 'class',
            theme: {
                extend: {
                    colors: {
                        'custom-bg': '#0f172a',
                        'custom-card': '#1e293b',
                        'custom-border': '#334155'
                    }
                }
            }
        }
    </script>
</head> 

<body class="bg-slate-950 text-slate-100 min-h-screen">
    <div class="max-w-6xl mx-auto px-6 py-12">
        <!-- Header Section -->
        <div class="flex items-center gap-8 mb-12">
            <div class="flex-shrink-0">
                <!-- Display the SVG logo -->
                <img src="logo.svg" alt="Logo" class="h-12 w-auto max-w-none">
            </div>
            <div class="flex-1">
                <h1 class="text-4xl font-bold text-white mb-2">Releases</h1>
                <p class="text-slate-400 text-lg">
                    See release notes at 
                    <a href="https://gitlab.com/kelteseth/ScreenPlay/-/releases" 
                       class="text-blue-400 hover:text-blue-300 underline decoration-blue-400/30 hover:decoration-blue-300/50 transition-colors">
                        Gitlab
                    </a>.
                </p>
            </div>
        </div>

        <?php
        $releaseDir = './releases';

        // Function to get modification date from SHA512-SUMS.txt file
        function getModificationDate($folder, $releaseDir) {
            $hashFile = $releaseDir . '/' . $folder . '/SHA512-SUMS.txt';
            if (file_exists($hashFile)) {
                return filemtime($hashFile);
            }
            // Fallback to folder modification time
            return filemtime($releaseDir . '/' . $folder);
        }

        if (is_dir($releaseDir)) {
            $folders = array_diff(scandir($releaseDir), ['.', '..']);

            // Sort by date (newest releases first)
            usort($folders, function ($a, $b) use ($releaseDir) {
                $modTimeA = getModificationDate($a, $releaseDir);
                $modTimeB = getModificationDate($b, $releaseDir);
                return $modTimeB - $modTimeA;
            });

            foreach ($folders as $folder) {
                echo '<div class="bg-slate-800/50 border border-slate-700 rounded-xl p-6 mb-8 shadow-2xl">';
                echo '<div class="flex items-center gap-3 mb-4">';
                echo '<h3 class="text-2xl font-semibold text-white">' . htmlspecialchars($folder);
                // Check for semver suffix and add pre-release label
                if (preg_match('/-(dev|beta|alpha|rc)[0-9]+$/', $folder)) {
                    echo ' <span class="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium bg-amber-500/20 text-amber-400 border border-amber-500/30">Pre-release</span>';
                }
                echo '</h3>';
                echo '</div>';

                $folderPath = $releaseDir . '/' . $folder;
                $modTime = getModificationDate($folder, $releaseDir);
                // Display "Uploaded" date and "Release Notes" button on the same line
                echo '<div class="flex items-center gap-4 mb-6">';
                echo '<p class="text-slate-400 text-sm">Uploaded ' . date("d.m.Y", $modTime) . '</p>';
                $releaseNoteLink = "https://gitlab.com/kelteseth/ScreenPlay/-/releases/" . urlencode($folder);
                echo "<a href=\"$releaseNoteLink\" target=\"_blank\" class=\"inline-flex items-center text-sm text-blue-400 hover:text-blue-300 underline decoration-blue-400/30 hover:decoration-blue-300/50 transition-colors\">Release Notes</a>";
                echo '</div>';


                $files = scandir($releaseDir . '/' . $folder);
                echo '<div class="space-y-2">';
                foreach ($files as $file) {
                    $filePath = $releaseDir . '/' . $folder . '/' . $file;
                    $fileExt = pathinfo($file, PATHINFO_EXTENSION);

                    if ($file !== '.' && $file !== '..' && ($fileExt == 'zip' || $fileExt == 'dmg')) {
                        $icon = "";
                        $platformClass = "";
                        if (strpos($file, 'osx-universal') !== false) {
                            $icon = "🍏 MacOS universal (unsigned) ";
                            $platformClass = "border-l-green-500";
                        } elseif (strpos($file, 'windows') !== false) {
                            $icon = "🪟 Windows x64 ";
                            $platformClass = "border-l-blue-500";
                        } elseif (strpos($file, 'linux') !== false) {
                            $icon = "🐧 Linux x64 ";
                            $platformClass = "border-l-orange-500";
                        }

                        echo '<div class="bg-slate-800 border border-slate-600 ' . $platformClass . ' border-l-4 rounded-lg p-4 hover:bg-slate-700 transition-colors">';
                        echo '<a href="' . htmlspecialchars($filePath) . '" class="flex items-center text-slate-200 hover:text-white no-underline">';
                        echo '<span class="mr-3">' . $icon . '</span>';
                        echo '<span class="font-mono text-sm">' . htmlspecialchars($file) . '</span>';
                        echo '</a>';
                        echo '</div>';
                    }
                }

                // Display download link for SHA512-SUMS.txt
                $hashFile = $releaseDir . '/' . $folder . '/SHA512-SUMS.txt';
                if (file_exists($hashFile)) {
                    echo '<div class="bg-slate-700/50 border border-slate-600 border-l-4 border-l-cyan-500 rounded-lg p-4 mt-2">';
                    echo '<a href="' . htmlspecialchars($hashFile) . '" class="flex items-center text-cyan-400 hover:text-cyan-300 no-underline transition-colors">';
                    echo '<span class="mr-3">ℹ️</span>';
                    echo '<span class="text-sm">Download SHA512-SUMS.txt</span>';
                    echo '</a>';
                    echo '</div>';
                }

                echo '</div>'; // Close space-y-2
                echo '</div>'; // Close main card
            }
        } else {
            echo "<div class='bg-red-900/20 border border-red-800 text-red-400 px-4 py-3 rounded-lg'>The 'releases' folder does not exist.</div>";
        }
        ?>
    </div>
</body>

</html>