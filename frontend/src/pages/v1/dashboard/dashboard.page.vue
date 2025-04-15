<template>
	<div
		class="widget col-span-2 sm:col-span-1 flex flex-col items-start p-4 bg-white border-none rounded-sm shadow-md cursor-text">

		<div class="widget-wrapper w-full widget-button flex items-center">
			<button class="widget-wrapper widget-button flex items-center">
				<i class="fas fa-wifi widget-icon w-16 h-14 !text-3.5xl leading-none box-border p-3 rounded-md mr-4"
					:class="wlanStatus ? 'text-black' : 'text-gray-400'"></i>
				<div class="widget-text flex-col items-start text-left text-gray-400">
					<h3 class="widget-text-header mb-1">WLAN</h3>
					<p class="widget-state text-xs text-left" id="light">{{ wlanStatusText }}</p>
				</div>
			</button>
		</div>
		<div class="widget-footer w-full mt-4 flex items-center justify-between border-t border-gray-300 pt-2">
			<span class="text-xs text-gray-500">{{ updateText }}</span>
			<span v-if="error" class="text-xs text-red-500">{{ err.message }}</span>
		</div>
	</div>
	<div
		class="widget col-span-2 sm:col-span-1 flex flex-col items-start p-4 bg-white border-none rounded-sm shadow-md cursor-text">

		<div class="widget-wrapper w-full widget-button flex items-center">
			<button class="widget-wrapper widget-button flex items-center">
				<i
					class="fas fa-code-branch widget-icon w-16 h-14 !text-3.5xl leading-none box-border p-3 rounded-md mr-4"></i>
				<div class="widget-text flex-col items-start text-left text-gray-400">
					<h3 class="widget-text-header mb-1">Version</h3>
					<p v-if="!isLoading" class="widget-state text-xs text-left">{{ systemVersion }}</p>
					<p v-else class="widget-state text-xs text-left">Version wird geladen...
					</p>
				</div>
			</button>
		</div>
		<div class="widget-footer w-full mt-4 flex items-center justify-between border-t border-gray-300 pt-2">
			<span class="text-xs text-gray-500">{{ updateText }}</span>
			<span v-if="available && !error" class="text-xs text-green-500">Neue Version verfügbar!</span>
			<span v-else-if="error" class="text-xs text-red-500">{{ err.message }}</span>
		</div>
	</div>

	<!-- <section class="p-3 grid grid-cols-2 gap-x-6 gap-y-6 sm:grid-cols-1 sm:gap-y-3"> -->
	<!-- <section
		class="p-3 grid grid-cols-[33%_33%33%] gap-x-6 gap-y-6 flex-wrap :p-[12px_0] sm:grid-cols-[100%] sm:gap-y-3"> -->
	<!-- <wlan-widget /> -->
	<!-- <version-widget /> -->

	<!-- Terminal Widget -->
	<!-- <div
		class="widget col-span-2 sm:col-span-1 flex flex-col items-start p-4 bg-white border-none rounded-sm shadow-md cursor-text">
		<div class="widget-wrapper widget-output w-full flex flex-col p-1">
			<div class="widget-header mb-2">
				<span class="text-xs text-gray-500">user@terminal:</span>
				<span class="text-xs text-gray-500">~$</span>
			</div>
			<div class="widget-content">
				<p class="widget-line leading-none text-sm">Welcome to the terminal widget!</p>
				<p class="widget-line leading-none text-sm">Run commands here to explore.</p>
				<p class="widget-line leading-none text-sm">System Status: <span class="text-yellow-300">ONLINE</span>
				</p>
			</div>
			<div class="widget-input mt-2">
				<span class="text-xs text-gray-500">user@terminal:</span>
				<input class="bg-black text-green-400 text-sm outline-none border-none p-0 ml-1" type="text"
					placeholder="Enter command..." />
			</div>
		</div>
	</div> -->

	<!-- <div
		class="widget col-span-2 sm:col-span-1 flex flex-col items-start p-4 bg-white border-none rounded-sm shadow-md cursor-text">
		<div class="widget-wrapper widget-output w-full flex flex-col p-1">
			<div class="widget-header mb-2">
				<span class="text-xs text-gray-500">user@terminal:</span>
				<span class="text-xs text-gray-500">~$</span>
			</div>
			<div class="widget-content">
				<p v-for="(line, index) in outputLines" :key="index" class="widget-line leading-none text-sm">
					{{ line }}
				</p>
			</div>
			<div class="widget-input mt-2">
				<span class="text-xs text-gray-500">user@terminal:</span>
				<input v-model="command" @keyup.enter="sendCommand"
					class="bg-black text-green-400 text-sm outline-none border-none p-0 ml-1" type="text"
					placeholder="Enter command..." />
			</div>
		</div>
	</div> -->
	<!-- Custom Widget -->
	<!-- <div class="widget flex flex-col p-4 bg-white border border-gray-200 rounded-md shadow-md">

		<div class="widget-header flex items-center justify-between mb-4">
			<h3 class="text-lg font-semibold text-gray-800">Widget Header</h3>
			<button class="text-gray-500 hover:text-gray-800 focus:outline-none">
				<i class="fas fa-ellipsis-h"></i>
			</button>
		</div>

		<div class="widget-body flex-grow text-gray-600">
			<p class="text-sm">This is the body content of the widget. You can customize it as needed.</p>
			<ul class="list-disc ml-5 mt-2 text-sm">
				<li>Dynamic content</li>
				<li>Customizable layout</li>
				<li>Flexible design</li>
			</ul>
		</div>

		<div class="widget-footer mt-4 flex items-center justify-between border-t border-gray-300 pt-2">
			<span class="text-xs text-gray-500">Last updated: 2 hours ago</span>
			<button class="text-sm text-blue-500 hover:text-blue-700 focus:outline-none">
				Action
			</button>
		</div>
	</div> -->
	<!-- </section> -->
</template>

<script src="./dashboard.page.ts"></script>
