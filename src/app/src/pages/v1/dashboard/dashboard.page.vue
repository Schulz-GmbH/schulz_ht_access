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
</template>

<script src="./dashboard.page.ts"></script>
