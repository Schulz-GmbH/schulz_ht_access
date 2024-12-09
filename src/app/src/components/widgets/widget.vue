<template>
	<div class="widget flex flex-col items-start p-2 bg-white border-none rounded-sm shadow-md cursor-pointer">
		<!-- Header -->
		<div v-if="!noHeader" class="widget-header w-full flex items-center justify-between mb-4">
			<h3 class="text-lg font-semibold text-gray-800">{{ title }}</h3>
			<button class="text-gray-500 hover:text-gray-800 focus:outline-none" @click="onMenuClick">
				<i class="fas fa-ellipsis-h"></i>
			</button>
		</div>

		<!-- Body -->

		<div class="widget-wrapper w-full widget-button flex items-center" v-on="$attrs">
			<slot name="body">
				<p class="text-sm">This is the body content of the widget. You can customize it as needed.</p>
			</slot>
		</div>


		<!-- Footer -->
		<div v-if="!noFooter"
			class="widget-footer w-full mt-4 flex items-center justify-between border-t border-gray-300 pt-2">
			<slot name="footer">
				<span class="text-xs text-gray-500">{{ footerText }}</span>
				<button class="text-sm text-blue-500 hover:text-blue-700 focus:outline-none" @click="onActionClick">
					{{ actionLabel }}
				</button>
			</slot>
		</div>
	</div>
</template>

<script lang="ts" src="./widget.ts"></script>
