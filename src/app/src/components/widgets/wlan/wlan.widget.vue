<template>
	<Widget :title="title" :footerText="footerText" :actionLabel="actionLabel" noHeader :noFooter="false"
		@click="toggleWlan">
		<template v-slot:body>
			<i class="fas fa-wifi widget-icon w-16 h-14 !text-3.5xl leading-none box-border p-3 rounded-md mr-4"
				:class="wlanStatus ? 'text-black' : 'text-gray-400'"></i>
			<div class="widget-text flex-col items-start text-gray-400">
				<h3 class="widget-text-header mb-1">WLAN</h3>
				<p class="widget-state text-xs text-left" id="light">{{ wlanStatusText }}</p>
			</div>
		</template>
		<template v-slot:footer>
			<span class="text-xs text-gray-500">{{ updateText }}</span>
		</template>
	</Widget>
</template>

<script lang="ts" src="./wlan.widget.ts"></script>
