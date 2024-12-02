<template>
	<section class="p-3 grid grid-cols-[49%_49%] gap-x-6 gap-y-6 flex-wrap :p-[12px_0] sm:grid-cols-[100%] sm:gap-y-3">
		<div class="widget flex items-center p-2 bg-white border-none rounded-sm shadow-md cursor-pointer">
			<button class="widget-wrapper widget-button flex items-center ">
				<i
					class="far fa-lightbulb widget-icon w-16 h-14 !text-3.5xl leading-none box-border p-3 rounded-md mr-4"></i>
				<div class="widget-text flex-col items-start text-gray-400">
					<h3 class="widget-text-header mb-1">Light</h3>
					<p class="widget-state text-xs text-left" id="light">OFF</p>
				</div>
			</button>
		</div>
		<div class="widget flex items-center p-2 bg-white border-none rounded-sm shadow-md cursor-pointer">
			<button class="widget-wrapper widget-button flex items-center ">
				<i
					class="fas fa-border-all widget-icon w-16 h-14 !text-3.5xl leading-none box-border p-3 rounded-md mr-4"></i>
				<div class="widget-text flex-col items-start text-gray-400">
					<h3 class="widget-text-header mb-1">Roller Shades</h3>
					<p class="widget-state text-xs text-left" id="shades">OFF</p>
				</div>
			</button>
		</div>
		<div class="widget flex items-center p-2 bg-white border-none rounded-sm shadow-md cursor-pointer">
			<button class="widget-wrapper widget-button flex items-center ">
				<i
					class="fas fa-volume-up widget-icon w-16 h-14 !text-3.5xl leading-none box-border p-3 rounded-md mr-4"></i>
				<div class="widget-text flex-col items-start text-gray-400">
					<h3 class="widget-text-header mb-1">Wireless Audio</h3>
					<p class="widget-state text-xs text-left" id="audio">OFF</p>
				</div>
			</button>
		</div>
		<div class="widget flex items-center p-2 bg-white border-none rounded-sm shadow-md cursor-pointer">
			<button class="widget-wrapper widget-button flex items-center ">
				<i
					class="fas fa-mug-hot widget-icon w-16 h-14 !text-3.5xl leading-none box-border p-3 rounded-md mr-4"></i>
				<div class="widget-text flex-col items-start text-gray-400">
					<h3 class="widget-text-header mb-1">Coffee Maker</h3>
					<p class="widget-state text-xs text-left" id="coffee">OFF</p>
				</div>
			</button>
		</div>
		<div
			class="widget col-span-2 sm:col-span-1 flex items-center p-2 bg-white border-none rounded-sm shadow-md cursor-pointer">
			test
		</div>
		<div
			class="widget col-span-2 sm:col-span-1 flex flex-col items-start p-4 bg-white border-none rounded-sm shadow-md cursor-text">
			<div class="widget-wrapper widget-output w-full flex flex-col p-1">
				<div class="widget-header mb-2">
					<span class="text-xs text-gray-500">user@terminal:</span>
					<span class="text-xs text-gray-500">~$</span>
				</div>
				<div class="widget-content">
					<p class="widget-line leading-none text-sm">Welcome to the terminal widget!</p>
					<p class="widget-line leading-none text-sm">Run commands here to explore.</p>
					<p class="widget-line leading-none text-sm">System Status: <span
							class="text-yellow-300">ONLINE</span></p>
				</div>
				<div class="widget-input mt-2">
					<span class="text-xs text-gray-500">user@terminal:</span>
					<input class="bg-black text-green-400 text-sm outline-none border-none p-0 ml-1" type="text"
						placeholder="Enter command..." />
				</div>
			</div>
		</div>

		<div class="widget flex flex-col p-4 bg-white border border-gray-200 rounded-md shadow-md">
			<!-- Header -->
			<div class="widget-header flex items-center justify-between mb-4">
				<h3 class="text-lg font-semibold text-gray-800">Widget Header</h3>
				<button class="text-gray-500 hover:text-gray-800 focus:outline-none">
					<i class="fas fa-ellipsis-h"></i>
				</button>
			</div>

			<!-- Body -->
			<div class="widget-body flex-grow text-gray-600">
				<p class="text-sm">This is the body content of the widget. You can customize it as
					needed.
				</p>
				<ul class="list-disc ml-5 mt-2 text-sm">
					<li>Dynamic content</li>
					<li>Customizable layout</li>
					<li>Flexible design</li>
				</ul>
			</div>

			<!-- Footer -->
			<div class="widget-footer mt-4 flex items-center justify-between border-t border-gray-300 pt-2">
				<span class="text-xs text-gray-500">Last updated: 2 hours ago</span>
				<button class="text-sm text-blue-500 hover:text-blue-700 focus:outline-none">
					Action
				</button>
			</div>
		</div>
	</section>

	<section class="weather">
		<nav class="weather__nav">
			<span class="weather__nav--active">TEMPERATURE</span>
			<span>HUMIDITY</span>
		</nav>

		<div class="weather__temp">
			<button class="fas fa-power-off weather__power" aria-label="power"></button>
			<svg class="weather__circle" viewbox="0 0 33.83098862 33.83098862" width="200" height="200"
				xmlns="http://www.w3.org/2000/svg">
				<circle class="weather__circle-state" stroke="#edf1f7" stroke-width="2" stroke-dasharray="72,72"
					stroke-linecap="round" fill="none" cx="16.91549431" cy="16.91549431" r="15.91549431" />
				<circle class="weather__circle-background" stroke="#edf1f7" stroke-width=".2" fill="none"
					cx="16.91549431" cy="16.91549431" r="15.91549431" />
				<circle class="weather__circle-fill" stroke="#3166ff" stroke-width="2" stroke-dasharray="45,72"
					stroke-linecap="round" fill="none" cx="16.91549431" cy="16.91549431" r="15.91549431" />
				<g class="weather__circle-info">
					<text class="weather__amount" x="16.91549431" y="14.91549431" alignment-baseline="central"
						text-anchor="middle" font-size="6"></text>
					<text class="weather__degrees" x="16.91549431" y="18.91549431" alignment-baseline="central"
						text-anchor="middle" font-size="2"></text>
					<text class="weather__null" x="16.91549431" y="16.91549431" alignment-baseline="central"
						text-anchor="middle" font-size="6" fill="#8f9bb3">--</text>
				</g>
			</svg>
		</div>

		<div class="weather-options">
			<button class="far fa-snowflake weather-options__option weather-options__option--active"></button>
			<button class="fas fa-sun weather-options__option"></button>
			<button class="fab fa-hotjar weather-options__option"></button>
			<button class="fas fa-sync-alt weather-options__option"></button>
		</div>
	</section>

</template>

<script src="./dashboard.page.ts"></script>
